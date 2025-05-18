#include "pmt/util/smrt/generic_lexer.hpp"

#include "pmt/base/numeric_cast.hpp"
#include "pmt/util/smrt/lexer_tables_base.hpp"

#include <cassert>
#include <utility>

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

 void set_bit(Bitset::ChunkSpan dest_, size_t index_) {
  size_t const chunk_index = index_ / Bitset::ChunkBit;
  size_t const bit_index = index_ % Bitset::ChunkBit;

  assert(chunk_index < dest_.size());
  dest_[chunk_index] |= Bitset::ChunkType(1) << bit_index;
 }

 auto get_bit(Bitset::ChunkSpanConst dest_, size_t index_) -> bool {
  size_t const chunk_index = index_ / Bitset::ChunkBit;
  size_t const bit_index = index_ % Bitset::ChunkBit;

  if (chunk_index >= dest_.size()) {
    return false;
  }

  return (dest_[chunk_index] & (Bitset::ChunkType(1) << bit_index)) != 0;
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
 : _accept_count(lexer_tables_.get_accept_count())
 , _accepts_all(Bitset::get_required_chunk_count(_accept_count), Bitset::ALL_SET_MASKS[true])
 , _accepts_valid(Bitset::get_required_chunk_count(_accept_count), Bitset::ALL_SET_MASKS[false])
 , _lexer_tables(lexer_tables_)
 , _input(input_)
 , _cursor(0)
 , _state_nr_newline(StateNrStart)
{
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

    if (_lexer_tables.is_linecount_state_nr_accepting(_state_nr_newline)) {
      _source_position._lineno++;
      _source_position._colno = 0;
      _state_nr_newline = StateNrStart;
    } else {
      _source_position._colno++;
    }

    bitwise_and(_accepts_valid, _lexer_tables.get_state_accepts(state_nr_cur), accepts_);

    countl = find_first_set_bit(_accepts_valid);

    if (countl < _accept_count) {
     if (countl == _lexer_tables.get_start_accept_index()) {
      _cursor = p;
     }

     te = p;
     id = _lexer_tables.get_accept_index_id(countl);
    }

    if (p == _input.size() + 1) {
      break;
    }

    SymbolType const symbol = (p == _input.size()) ? SymbolValueEoi : NumericCast::cast<SymbolType>(_input[p]);
    state_nr_cur = _lexer_tables.get_state_nr_next(state_nr_cur, symbol);
    _state_nr_newline = _lexer_tables.get_linecount_state_nr_next(_state_nr_newline, symbol);
  }

  if (id != GenericId::IdUninitialized && countl < _accept_count && countl != _lexer_tables.get_start_accept_index()) {
   LexReturn ret;
   ret._accepted = countl;
   ret._token._token = std::string_view(_input.data() + _cursor, te - _cursor);
   ret._token._id = id;
   ret._token._source_position = _source_position;
   _cursor = te;
   return ret;
  } else {
   std::string message = "Lexing error, expected to match: ";
   std::string delim;
   for (size_t i = 0; i < _accept_count; ++i) {
    if (get_bit(accepts_, i)) {
     message += std::exchange(delim, ", ") + "'" + _lexer_tables.get_accept_index_label(i) + "'";
    }
   }

   message += " at position " + std::to_string(_cursor);
   throw std::runtime_error(message);
  }
}

auto GenericLexer::get_eoi_accept_index() const -> size_t {
 return _lexer_tables.get_eoi_accept_index();
}

auto GenericLexer::get_accept_index_hide(size_t index_) const -> bool {
 return _lexer_tables.get_accept_index_hide(index_);
}

}  // namespace pmt::util::smrt