#include "pmt/util/parsert/generic_lexer.hpp"

#include "pmt/base/numeric_cast.hpp"
#include "pmt/util/parsert/generic_fa_state_tables.hpp"
#include "pmt/util/parsert/generic_tables_base.hpp"

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
 , _buffer(input_.data())
 , _buffer_size(input_.size())
 , _cursor(0) {
}

auto GenericLexer::lex() -> LexReturn {
  return lex(_accepts_all);
}

auto GenericLexer::lex(DynamicBitset const& accepts_) -> LexReturn {
  LexReturn ret;

  GenericId::IdType id = GenericId::IdUninitialized;
  size_t countl = std::numeric_limits<size_t>::max();
  size_t te = _cursor;  // Token end

  uint64_t state_nr_cur = GenericFaStateTables::StateNrStart;

  for (size_t p = _cursor; p <= _buffer_size; ++p) {
    if (state_nr_cur == _tables._fa_state_tables._state_nr_sink) {
      break;
    }

    _accepts_valid = _tables._accepts[state_nr_cur].clone_and(accepts_);

    assert(_accepts_valid.popcnt() <= 1);

    countl = _accepts_valid.countl(false);
    if (countl < _accepts_valid.size()) {
      te = p;
      id = _tables._accept_ids[countl];
    }

    if (state_nr_cur == _tables._fa_state_tables._state_nr_sink) {
      break;
    }

    GenericTablesBase::TableIndexType const symbol = (p == _buffer_size) ? GenericTablesBase::SYMBOL_EOI : NumericCast::cast<GenericTablesBase::TableIndexType>(_buffer[p]);
    state_nr_cur = _tables._fa_state_tables.get_state_nr_next(state_nr_cur, symbol);
  }

  if (id != GenericId::IdUninitialized) {
    ret._accepted = countl;
    ret._token._token = std::string_view(_buffer + _cursor, te - _cursor);
    ret._token._id = id;
    _cursor = te;
    return ret;
  } else {
    throw std::runtime_error("Unexpected character");
  }
}

}  // namespace pmt::util::parsert