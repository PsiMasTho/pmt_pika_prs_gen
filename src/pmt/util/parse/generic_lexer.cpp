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
 : _input(input_)
 , _tables(tables_) {
}

auto GenericLexer::next_token(pmt::base::DynamicBitset const& accepts_) -> GenericAst::UniqueHandle {
  // Skip whitespace
  while (!_input.empty() && (std::strchr(" \t\r\n", _input.front()) != nullptr)) {
    _input.remove_prefix(1);
  }

  if (_input.empty()) {
    return make_eoi_token();
  }

  GenericAst::IdType id_longest = GenericAst::IdConstants::IdUninitialized;
  std::optional<size_t> size_longest = 0;

  Fa::StateNrType state_nr_cur = _tables._state_nr_start;

  for (size_t i = 0; i < _input.size(); ++i) {
    char const c = _input[i];

    pmt::base::DynamicBitset const accepts_valid = accepts_.clone_and(_tables._accepts[state_nr_cur]);
    assert(accepts_valid.popcnt() <= 1);
    size_t const countl = accepts_valid.countl(false);

    if (countl == accepts_.size()) {
      state_nr_cur = _tables._transitions[state_nr_cur][c];
      if (state_nr_cur == GenericLexerTables::INVALID_STATE_NR) {
        if (size_longest.has_value()) {
          _input.remove_prefix(*size_longest);
          GenericAst::UniqueHandle ret = GenericAst::construct(GenericAst::Tag::Token, id_longest);
          ret->set_token(std::string(_input.data(), *size_longest));
          return ret;
        } else {
          throw std::runtime_error("Unexpected character: " + std::string(1, c));
        }
      }
    } else {
      size_longest = i + 1;
      id_longest = _tables._terminals[countl]._id;

      state_nr_cur = _tables._transitions[state_nr_cur][c];
      if (state_nr_cur == GenericLexerTables::INVALID_STATE_NR) {
        _input.remove_prefix(i);
        GenericAst::UniqueHandle ret = GenericAst::construct(GenericAst::Tag::Token, id_longest);
        ret->set_token(std::string(_input.data(), i));
        return ret;
      }
    }
  }

  _input.remove_prefix(_input.size());
  return make_eoi_token();
}

}  // namespace pmt::util::parse