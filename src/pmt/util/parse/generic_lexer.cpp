#include "pmt/util/parse/generic_lexer.hpp"

#include "pmt/base/dynamic_bitset.hpp"

#include <algorithm>
#include <cassert>
#include <cstring>
#include <numeric>
#include <span>

namespace pmt::util::parse {
namespace {
auto make_eoi_token() -> GenericAst::UniqueHandle {
  auto ret = GenericAst::construct(GenericAst::Tag::Token, GenericAst::IdConstants::IdEoi);
  ret->set_token("EOI");
  return ret;
}

auto raw_bitset_and(std::span<uint64_t const> lhs_, std::span<uint64_t const> rhs_, std::span<uint64_t> out_) {
  assert(lhs_.size() == rhs_.size());
  assert(lhs_.size() == out_.size());

  for (size_t i = 0; i < lhs_.size(); ++i) {
    out_[i] = lhs_[i] & rhs_[i];
  }
}

auto raw_bitset_find_first_bit(std::span<uint64_t const> chunks_) -> size_t {
  size_t total = 0;
  for (uint64_t const chunk : chunks_) {
    size_t const incr = std::countr_zero(chunk);
    total += incr;
    if (incr != sizeof(uint64_t) * CHAR_BIT) {
      break;
    }
  }

  return total;
}

auto raw_bitset_popcnt(std::span<uint64_t const> chunks_) -> size_t {
  return std::accumulate(chunks_.begin(), chunks_.end(), 0, [](size_t acc_, uint64_t chunk_) { return acc_ + std::popcount(chunk_); });
}

}  // namespace

GenericLexer::GenericLexer(std::string_view input_, GenericLexerTables const& tables_)
 : _begin(input_.data())
 , _cursor(input_.data())
 , _end(input_.data() + input_.size())
 , _tables(tables_) {
}

auto GenericLexer::next_token(std::span<uint64_t const> accepts_) -> GenericAst::UniqueHandle {
  while (_cursor != _end && (std::strchr(" \t\r\n", *_cursor) != nullptr)) {
    ++_cursor;
  }

  if (_cursor == _end) {
    return make_eoi_token();
  }

  GenericAst::IdType id = GenericAst::IdConstants::IdUninitialized;
  char const* te = nullptr;  // Token end

  uint64_t state_nr_cur = GenericLexerTables::STATE_NR_START;
  std::vector<uint64_t> accepts_valid(pmt::base::DynamicBitset::get_required_chunk_count(_tables._terminal_ids.size()), 0);

  for (char const* p = _cursor; _cursor <= _end; ++p) {
    if (state_nr_cur == _tables._state_nr_invalid) {
      if (te != nullptr) {
        GenericAst::UniqueHandle ret = GenericAst::construct(GenericAst::Tag::Token, id);
        ret->set_token(std::string(_cursor, te));
        _cursor = te;
        return ret;
      } else {
        throw std::runtime_error("Unexpected character: " + std::string(1, *_cursor));
      }
    }

    std::ranges::fill(accepts_valid, 0);
    raw_bitset_and(_tables._accepts[state_nr_cur], accepts_, accepts_valid);
    assert(raw_bitset_popcnt(accepts_valid) <= 1);

    if (size_t const countl = raw_bitset_find_first_bit(accepts_valid); countl < _tables._terminal_ids.size()) {
      te = p;
      id = _tables._terminal_ids[countl];

      if (state_nr_cur == _tables._state_nr_invalid) {
        GenericAst::UniqueHandle ret = GenericAst::construct(GenericAst::Tag::Token, id);
        ret->set_token(std::string(_cursor, te));
        _cursor = te;
        return ret;
      }
    }

    if (p == _end) {
      break;
    }

    uint64_t const offset = _tables._transitions_shifts[state_nr_cur] + *p;
    state_nr_cur = (offset >= _tables._transitions_check.size()) ? _tables._state_nr_invalid : (_tables._transitions_check[offset] == state_nr_cur) ? _tables._transitions_next[offset] : _tables._state_nr_invalid;
  }

  if (te != nullptr) {
    GenericAst::UniqueHandle ret = GenericAst::construct(GenericAst::Tag::Token, id);
    ret->set_token(std::string(_cursor, te));
    _cursor = te;
    return ret;
  } else {
    throw std::runtime_error("Unexpected character: " + std::string(1, *_cursor));
  }
}

}  // namespace pmt::util::parse