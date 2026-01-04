#include "pmt/parser/rt/generic_lexer.hpp"

#include "pmt/base/numeric_cast.hpp"
#include "pmt/parser/primitives.hpp"
#include "pmt/parser/rt/lexer_tables_base.hpp"
#include "pmt/parser/rt/util.hpp"

#include <cassert>
#include <utility>

namespace pmt::parser::rt {
using namespace pmt::base;
using namespace pmt::sm;

namespace {}

GenericLexer::GenericLexer(std::string_view input_, LexerTablesBase const& lexer_tables_)
 : _accepts_all(Bitset::get_required_chunk_count(lexer_tables_.get_accept_count()), Bitset::ALL_SET_MASKS[true])
 , _accepts_valid(Bitset::get_required_chunk_count(lexer_tables_.get_accept_count()), Bitset::ALL_SET_MASKS[false])
 , _lexer_tables(lexer_tables_)
 , _input(input_)
 , _cursor(0)
 , _linecount_cursor(0)
 , _state_nr_linecount(StateNrStart) {
}

auto GenericLexer::lex() -> LexReturn {
 return lex(_accepts_all);
}

auto GenericLexer::lex(Bitset::ChunkSpanConst accepts_) -> LexReturn {
 LexReturn real_match;
 LexReturn fallback_match;

 StateNrType state_nr_cur = StateNrStart;

 for (size_t p = _cursor; p <= _input.size() + 1; ++p) {
  if (state_nr_cur == StateNrInvalid) {
   break;
  }

  Bitset::ChunkSpanConst const state_accepts = _lexer_tables.get_state_accepts(state_nr_cur);
  bitwise_and(_accepts_valid, state_accepts, accepts_);
  size_t const first_set_bit_real = find_first_set_bit(_accepts_valid);

  if (get_bit(state_accepts, _lexer_tables.get_start_accept_index())) {
   _cursor = p;
  } else if (first_set_bit_real < _lexer_tables.get_accept_count()) {
   real_match = create_lex_return(p, first_set_bit_real);
  } else {
   size_t const first_set_bit_fallback = find_first_set_bit(state_accepts);
   if (first_set_bit_fallback < _lexer_tables.get_accept_count() && first_set_bit_fallback != _lexer_tables.get_start_accept_index()) {
    fallback_match = create_lex_return(p, first_set_bit_fallback);
   }
  }

  if (p == _input.size() + 1) {
   break;
  }

  SymbolType const symbol = (p == _input.size()) ? SymbolValueEoi : NumericCast::cast<SymbolType>(_input[p]);
  state_nr_cur = _lexer_tables.get_state_nr_next(state_nr_cur, symbol);
 }

 if (is_lex_return_valid(real_match)) {
  _cursor += real_match._token._token.size();
  return real_match;
 }

 // The fallback may be invalid too, in that case set the source position so we can at least report that
 if (!is_lex_return_valid(fallback_match)) {
  fallback_match._token._source_position = get_source_position_at(_cursor);
 }

 throw_lexing_error(_lexer_tables, accepts_, fallback_match);
}

auto GenericLexer::get_tables() const -> LexerTablesBase const& {
 return _lexer_tables;
}

void GenericLexer::throw_lexing_error(LexerTablesBase const& lexer_tables_, pmt::base::Bitset::ChunkSpanConst accepts_expected_, LexReturn const& lex_return_) {
 std::string message = "Lexing error!\n";
 std::string delim = "Expected: ";
 size_t expected_reported = 0;
 size_t const expected = get_popcount(accepts_expected_);

 for (size_t i = 0; i < (accepts_expected_.size() * Bitset::ChunkBit); ++i) {
  if (expected_reported == EXPECTED_REPORTED_MAX && expected_reported < expected) {
   message += " or ... (" + std::to_string(expected - expected_reported) + " more)";
   break;
  }

  if (get_bit(accepts_expected_, i)) {
   message += std::exchange(delim, " or ") + lexer_tables_.get_accept_index_display_name(i);
   ++expected_reported;
  }
 }

 if (lex_return_._accepted < lexer_tables_.get_accept_count()) {
  message += (expected_reported != 0) ? "\n" : "";
  message += "Got: " + lexer_tables_.get_accept_index_display_name(lex_return_._accepted);
 }
 message += "\nAt line: " + std::to_string(lex_return_._token._source_position._lineno) + ", column: " + std::to_string(lex_return_._token._source_position._colno);
 throw std::runtime_error(message);
}

auto GenericLexer::get_source_position_at(size_t p_) -> SourcePosition {
 assert(_linecount_last <= p_);

 while (_linecount_cursor <= p_) {
  if (_lexer_tables.is_linecount_state_nr_accepting(_state_nr_linecount)) {
   _linecount_last = _linecount_cursor;
   ++_linecount_at_last;
   _state_nr_linecount = StateNrStart;
  }

  SymbolType const symbol = (p_ == _input.size()) ? SymbolValueEoi : NumericCast::cast<SymbolType>(_input[_linecount_cursor]);
  _state_nr_linecount = _lexer_tables.get_linecount_state_nr_next(_state_nr_linecount, symbol);
  ++_linecount_cursor;
 }

 return SourcePosition(_linecount_at_last, (_linecount_cursor - _linecount_last));
}

auto GenericLexer::create_lex_return(size_t p_, size_t countl_) -> LexReturn {
 LexReturn ret;

 ret._token._token = std::string_view(_input.data() + _cursor, p_ - _cursor);
 ret._token._id = _lexer_tables.get_accept_index_id(countl_);
 ret._token._source_position = get_source_position_at(_cursor);
 ret._accepted = countl_;

 return ret;
}

auto GenericLexer::is_lex_return_valid(LexReturn const& lex_return_) const -> bool {
 return lex_return_._token._id != GenericId::IdUninitialized && lex_return_._accepted < _lexer_tables.get_accept_count();
}

}  // namespace pmt::parser::rt