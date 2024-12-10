#include "pmt/util/parsert/generic_lexer.hpp"

#include "pmt/util/parsert/raw_bitset.hpp"

#include <algorithm>
#include <cassert>
#include <cstdint>
#include <cstring>

namespace pmt::util::parsert {

using RawBitsetType = RawBitset<GenericLexerTables::TableIndexType>;

GenericLexer::GenericLexer(std::string_view input_, GenericLexerTables const& tables_)
 : _accepts_valid(std::make_unique_for_overwrite<GenericLexerTables::TableIndexType[]>(RawBitsetType::get_required_chunk_count(tables_._accepts_2d_width)))
 , _accepts_all(std::make_unique_for_overwrite<GenericLexerTables::TableIndexType[]>(RawBitsetType::get_required_chunk_count(tables_._accepts_2d_width)))
 , _tables(tables_)
 , _begin(input_.data())
 , _cursor(input_.data())
 , _end(input_.data() + input_.size()) {
  fill_bitset(_accepts_valid.get(), false);
  fill_bitset(_accepts_all.get(), true);
}

auto GenericLexer::lex() -> LexReturn {
  return lex(_accepts_all.get());
}

auto GenericLexer::lex(GenericLexerTables::TableIndexType const* accepts_) -> LexReturn {
  while (_cursor != _end && (std::strchr(" \t\r\n", *_cursor) != nullptr)) {
    ++_cursor;
  }

  LexReturn ret;

  if (_cursor == _end) {
    ret._token._id = GenericId::IdEoi;
    return ret;
  }

  GenericId::IdType id = GenericId::IdUninitialized;
  size_t countl = std::numeric_limits<size_t>::max();
  char const* te = nullptr;  // Token end

  uint64_t state_nr_cur = GenericLexerTables::StateNrStart;

  for (char const* p = _cursor; _cursor <= _end; ++p) {
    if (state_nr_cur == _tables._state_nr_sink) {
      if (te != nullptr) {
        ret._accepted = countl;
        ret._token._token = std::string_view(_cursor, te);
        ret._token._id = id;
        _cursor = te;
        return ret;
      } else {
        throw std::runtime_error("Unexpected character: " + std::string(1, *_cursor));
      }
    }

    fill_bitset(_accepts_valid.get(), false);
    RawBitsetType::op_and(_tables.get_accepts_2d_bitset_at(state_nr_cur), accepts_, _accepts_valid.get(), _tables._accepts_2d_width);
    assert(RawBitsetType::popcnt(_accepts_valid.get(), _tables._accepts_2d_width) <= 1);

    countl = RawBitsetType::find_first_bit(_accepts_valid.get(), _tables._accepts_2d_width);
    if (countl < _tables._accepts_2d_width) {
      te = p;
      id = _tables._accept_ids[countl];

      if (state_nr_cur == _tables._state_nr_sink) {
        ret._accepted = countl;
        ret._token._token = std::string_view(_cursor, te);
        ret._token._id = id;
        _cursor = te;
        return ret;
      }
    }

    if (p == _end) {
      break;
    }

    state_nr_cur = _tables.get_next_state(state_nr_cur, *p);
  }

  if (te != nullptr) {
    ret._accepted = countl;
    ret._token._token = std::string_view(_cursor, te);
    ret._token._id = id;
    _cursor = te;
    return ret;
  } else {
    throw std::runtime_error("Unexpected character: " + std::string(1, *_cursor));
  }
}

void GenericLexer::fill_bitset(GenericLexerTables::TableIndexType* bitset_, bool value_) {
  std::fill(bitset_, bitset_ + RawBitsetType::get_required_chunk_count(_tables._accepts_2d_width), value_ ? std::numeric_limits<GenericLexerTables::TableIndexType>::max() : 0);
}

}  // namespace pmt::util::parsert