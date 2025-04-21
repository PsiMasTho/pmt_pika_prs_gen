#include "pmt/util/smrt/generic_lexer.hpp"

#include "pmt/base/numeric_cast.hpp"

#include <cassert>

namespace pmt::util::smrt {
using namespace pmt::base;

GenericLexer::GenericLexer(std::string_view input_, StateMachineTablesBase const& tables_)
 : _accepts_all(tables_.get_accept_count(), true)
 , _tables(tables_)
 , _input(input_)
 , _cursor(0) {
}

auto GenericLexer::lex() -> LexReturn {
  return lex(_accepts_all.get_chunks());
}

auto GenericLexer::lex(Bitset::ChunkSpanConst accepts_) -> LexReturn {
  Bitset accepts(accepts_);
  accepts.resize(_accepts_all.size(), false);

  GenericId::IdType id = GenericId::IdUninitialized;
  size_t countl = _accepts_all.size();
  size_t te = _cursor;  // Token end

  StateNrType state_nr_cur = StateNrStart;

  for (size_t p = _cursor; p <= _input.size(); ++p) {
    if (state_nr_cur == StateNrSink) {
      break;
    }

    Bitset state_accepts(_tables.get_state_accepts(state_nr_cur));
    if (state_accepts.size() < accepts.size()) {
      state_accepts.resize(accepts.size(), false);
    }

    Bitset const accepts_valid = state_accepts.clone_and(accepts);

    assert(accepts_valid.popcnt() <= 1);

    countl = accepts_valid.countl(false);
    if (countl < accepts_valid.size()) {
      te = p;
      id = _tables.get_accept_id(countl);
    }

    if (state_nr_cur == StateNrSink) {
      break;
    }

    SymbolType const symbol = (p == _input.size()) ? SymbolEoi : NumericCast::cast<SymbolType>(_input[p]);
    state_nr_cur = _tables.get_state_nr_next(state_nr_cur, symbol);
  }

  LexReturn ret;
  if (id != GenericId::IdUninitialized) {
    ret._accepted = countl;
    ret._token._token = std::string_view(_input.data() + _cursor, te - _cursor);
    ret._token._id = id;
    _cursor = te;
    return ret;
  } else {
    throw std::runtime_error("Unexpected character");
  }
}

}  // namespace pmt::util::smrt