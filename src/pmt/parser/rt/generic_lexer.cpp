#include "pmt/parser/rt/generic_lexer.hpp"

#include "pmt/base/numeric_cast.hpp"
#include "pmt/parser/primitives.hpp"
#include "pmt/parser/rt/lexer_tables_base.hpp"
#include "pmt/parser/rt/util.hpp"

#include <cassert>
#include <utility>

namespace pmt::parser::rt {
using namespace pmt::base;
using namespace pmt::util::sm;

namespace {}

GenericLexer::GenericLexer(std::string_view input_, LexerTablesBase const& lexer_tables_)
 : _accept_count(lexer_tables_.get_accept_count())
 , _accepts_all(Bitset::get_required_chunk_count(_accept_count), Bitset::ALL_SET_MASKS[true])
 , _accepts_valid(Bitset::get_required_chunk_count(_accept_count), Bitset::ALL_SET_MASKS[false])
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
  GenericId::IdType id = GenericId::IdUninitialized;
  size_t countl = _accept_count;
  size_t te = _cursor;  // Token end

  StateNrType state_nr_cur = StateNrStart;

  for (size_t p = _cursor; p <= _input.size() + 1; ++p) {
    if (state_nr_cur == StateNrSink) {
      break;
    }

    Bitset::ChunkSpanConst const state_accepts = _lexer_tables.get_state_accepts(state_nr_cur);
    bitwise_and(_accepts_valid, state_accepts, accepts_);
    countl = find_first_set_bit(_accepts_valid);

    if (get_bit(state_accepts, _lexer_tables.get_start_accept_index())) {
      _cursor = p;
    }

    if (countl < _accept_count) {
      te = p;
      id = _lexer_tables.get_accept_index_id(countl);
    }

    if (p == _input.size() + 1) {
      break;
    }

    SymbolValueType const symbol = (p == _input.size()) ? SymbolValueEoi : NumericCast::cast<SymbolValueType>(_input[p]);
    state_nr_cur = _lexer_tables.get_state_nr_next(state_nr_cur, symbol);
  }

  if (id != GenericId::IdUninitialized && countl < _accept_count) {
    LexReturn ret;
    ret._accepted = countl;
    ret._token._token = std::string_view(_input.data() + _cursor, te - _cursor);
    ret._token._id = id;
    ret._token._source_position = get_source_position_at(_cursor);
    _cursor = te;
    return ret;
  } else {
    std::string message = "Lexing error,";
    std::string delim = " expected to match: ";

    for (size_t i = 0; i < _accept_count; ++i) {
      if (get_bit(accepts_, i)) {
        message += std::exchange(delim, ", ") + "'" + _lexer_tables.get_accept_index_label(i) + "'";
      }
    }

    SourcePosition const source_position = get_source_position_at(_cursor);
    message += " at line: " + std::to_string(source_position._lineno) + ", column: " + std::to_string(source_position._colno);
    throw std::runtime_error(message);
  }
}

auto GenericLexer::get_eoi_accept_index() const -> size_t {
  return _lexer_tables.get_eoi_accept_index();
}

auto GenericLexer::get_source_position_at(size_t p_) -> SourcePosition {
  assert(_linecount_last <= p_);

  while (_linecount_cursor < p_) {
    if (_lexer_tables.is_linecount_state_nr_accepting(_state_nr_linecount)) {
      _linecount_last = _linecount_cursor;
      ++_linecount_at_last;
      _state_nr_linecount = StateNrStart;
    }

    SymbolValueType const symbol = (p_ == _input.size()) ? SymbolValueEoi : NumericCast::cast<SymbolValueType>(_input[_linecount_cursor]);
    _state_nr_linecount = _lexer_tables.get_linecount_state_nr_next(_state_nr_linecount, symbol);
    ++_linecount_cursor;
  }

  return SourcePosition(_linecount_at_last, (_linecount_cursor - _linecount_last) + 1);
}

}  // namespace pmt::parser::rt