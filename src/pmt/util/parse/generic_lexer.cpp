#include "pmt/util/parse/generic_lexer.hpp"

#include "pmt/base/dynamic_bitset.hpp"
#include "pmt/util/parse/generic_ast.hpp"
#include "pmt/util/parse/generic_lexer_tables.hpp"

#include <cassert>
#include <cstring>

namespace pmt::util::parse {
namespace {
auto make_eoi_token() -> GenericAst::UniqueHandle {
  auto ret = GenericAst::construct(GenericAst::Tag::Token, GenericAst::IdConstants::IdEoi);
  ret->set_token("EOI");
  return ret;
}
}  // namespace

GenericLexer::GenericLexer(std::string_view input_, GenericLexerTables const& tables_)
 : _begin(input_.data())
 , _cursor(input_.data())
 , _end(input_.data() + input_.size())
 , _tables(tables_) {
}

auto GenericLexer::next_token(pmt::base::DynamicBitset const& accepts_) -> GenericAst::UniqueHandle {
  while (_cursor != _end && (std::strchr(" \t\r\n", *_cursor) != nullptr)) {
    ++_cursor;
  }

  if (_cursor == _end) {
    return make_eoi_token();
  }

  GenericAst::IdType id = GenericAst::IdConstants::IdUninitialized;
  char const* te = nullptr;  // Token end

  Fa::StateNrType state_nr_cur = GenericLexerTables::STATE_NR_START;

  for (char const* p = _cursor; _cursor <= _end; ++p) {
    if (state_nr_cur == GenericLexerTables::STATE_NR_INVALID) {
      if (te != nullptr) {
        GenericAst::UniqueHandle ret = GenericAst::construct(GenericAst::Tag::Token, id);
        ret->set_token(std::string(_cursor, te));
        _cursor = te;
        return ret;
      } else {
        throw std::runtime_error("Unexpected character: " + std::string(1, *_cursor));
      }
    }

    pmt::base::DynamicBitset const accepts_valid = accepts_.clone_and(_tables._accepts[state_nr_cur]);
    assert(accepts_valid.popcnt() <= 1);
    size_t const countl = accepts_valid.countl(false);

    if (countl != accepts_.size()) {
      te = p;
      id = _tables._terminal_ids[countl];

      if (state_nr_cur == GenericLexerTables::STATE_NR_INVALID) {
        GenericAst::UniqueHandle ret = GenericAst::construct(GenericAst::Tag::Token, id);
        ret->set_token(std::string(_cursor, te));
        _cursor = te;
        return ret;
      }
    }

    if (p == _end) {
      break;
    }

    state_nr_cur = _tables._transitions[state_nr_cur][*p];
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