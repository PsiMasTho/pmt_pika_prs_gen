#include "pmt/util/smrt/generic_lexer.hpp"

#include "pmt/base/numeric_cast.hpp"
#include "pmt/util/smrt/lexer_tables_base.hpp"

#include <cassert>

namespace pmt::util::smrt {
using namespace pmt::base;

namespace {
 auto find_first_set_bit(Bitset::ChunkSpanConst const& bitset_) -> size_t {
  size_t total = 0;
  for (Bitset::ChunkType chunk : bitset_) {
    size_t const incr = std::countr_zero(chunk);
    total += incr;
    if (incr != Bitset::ChunkBit) {
      break;
    }
  }

  return total;
 }

 void bitwise_and(Bitset::ChunkSpan dest_, Bitset::ChunkSpanConst lhs_ , Bitset::ChunkSpanConst rhs_) {
  std::fill(dest_.begin(), dest_.end(), Bitset::ChunkType(0));

  Bitset::ChunkSpanConst const* smaller = lhs_.size() < rhs_.size() ? &lhs_ : &rhs_;
  Bitset::ChunkSpanConst const* larger = lhs_.size() < rhs_.size() ? &rhs_ : &lhs_;

  for (size_t i = 0; i < smaller->size(); ++i) {
    dest_[i] = (*smaller)[i] & (*larger)[i];
  }
 }
}

GenericLexer::GenericLexer(std::string_view input_, LexerTablesBase const& lexer_tables_)
 : _accept_count(lexer_tables_.get_terminal_count())
 , _accepts_all(Bitset::get_required_chunk_count(_accept_count), ~Bitset::ChunkType(0))
 , _accepts_valid(Bitset::get_required_chunk_count(_accept_count), Bitset::ChunkType(0))
 , _lexer_tables(lexer_tables_)
 , _input(input_)
 , _cursor(0)
 , _state_nr_newline(StateNrStart)
 , _state_nr_newline_accept(lexer_tables_.get_newline_state_nr_accept()) {
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

    if (_state_nr_newline == _state_nr_newline_accept) {
      _source_position._lineno++;
      _source_position._colno = 0;
      _state_nr_newline = StateNrStart;
    } else {
      _source_position._colno++;
    }

    bitwise_and(_accepts_valid, _lexer_tables.get_state_terminals(state_nr_cur), accepts_);

    countl = find_first_set_bit(_accepts_valid);

    if (countl < _accept_count) {
     if (countl == _lexer_tables.get_start_terminal_index()) {
      _cursor = p;
     }

     te = p;
     id = _lexer_tables.get_terminal_id(countl);
    }

    if (p == _input.size() + 1) {
      break;
    }

    SymbolType const symbol = (p == _input.size()) ? SymbolValueEoi : NumericCast::cast<SymbolType>(_input[p]);
    state_nr_cur = _lexer_tables.get_state_nr_next(state_nr_cur, symbol);
    _state_nr_newline = _lexer_tables.get_newline_state_nr_next(_state_nr_newline, symbol);
  }

  if (id != GenericId::IdUninitialized && countl < _accept_count && countl != _lexer_tables.get_start_terminal_index()) {
   LexReturn ret;
   ret._accepted = countl;
   ret._token._token = std::string_view(_input.data() + _cursor, te - _cursor);
   ret._token._id = id;
   ret._token._source_position = _source_position;
   _cursor = te;
   return ret;
  } else {
   throw std::runtime_error("Unexpected character");
  }
}

}  // namespace pmt::util::smrt