#include "pmt/util/parsert/generic_lexer.hpp"

#include <cassert>
#include <cstdint>
#include <cstring>
#include <limits>

namespace pmt::util::parsert {
using namespace pmt::base;

GenericLexer::GenericLexer(std::string_view input_, GenericLexerTables tables_)
 : _tables(std::move(tables_))
 , _accepts_valid(tables_._accepts_width, false)
 , _accepts_all(tables_._accepts_width, true)
 , _begin(input_.data())
 , _cursor(input_.data())
 , _end(input_.data() + input_.size()) {
}

auto GenericLexer::lex() -> LexReturn {
  return lex(_accepts_all);
}

auto GenericLexer::lex(DynamicBitset const& accepts_) -> LexReturn {
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
      break;
    }

    _accepts_valid = _tables._accepts[state_nr_cur].clone_and(accepts_);

    assert(_accepts_valid.popcnt() <= 1);

    countl = _accepts_valid.countl(false);
    if (countl < _accepts_valid.size()) {
      te = p;
      id = _tables._accept_ids[countl];
    }

    if (p == _end || state_nr_cur == _tables._state_nr_sink) {
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

}  // namespace pmt::util::parsert