#include "pmt/parser/grammar/permute.hpp"

#include "pmt/base/match.hpp"
#include "pmt/parser/generic_ast.hpp"
#include "pmt/parser/grammar/ast.hpp"
#include "pmt/parser/grammar/repetition_range.hpp"

#include <cassert>

namespace pmt::parser::grammar {
using namespace pmt::base;
namespace {}

Permute::Permute(GenericAst const &ast_)
 : _sequence_length(ast_.get_child_at_front()->get_children_size())
 , _min_items(0)
 , _max_items(0) {
  assert(ast_.get_id() == Ast::NtSequenceModifier);
  assert(Match::is_any_of(ast_.get_child_at_front()->get_id(), Ast::NtTerminalSequence, Ast::NtNonterminalSequence));

  if (ast_.get_child_at_back()->get_id() == Ast::NtRepetitionRange) {
    RepetitionRange const range(*ast_.get_child_at_back());
    _min_items = range.get_lower();
    _max_items = range.get_upper().value_or(_sequence_length);
  } else {
    _min_items = _sequence_length;
    _max_items = _sequence_length;
  }

  _min_items = std::min(_min_items, _max_items);

  if (_max_items > _sequence_length) {
    static std::string const ERROR_MSG = "Permute: Max items cannot be greater than sequence length.";
    throw std::runtime_error(ERROR_MSG);
  }
}

auto Permute::get_sequence_length() const -> size_t {
  return _sequence_length;
}

auto Permute::get_min_items() const -> size_t {
  return _min_items;
}

auto Permute::get_max_items() const -> size_t {
  return _max_items;
}

}  // namespace pmt::parser::grammar