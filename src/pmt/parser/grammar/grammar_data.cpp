#include "pmt/parser/grammar/grammar_data.hpp"

#include "pmt/asserts.hpp"
#include "pmt/base/algo.hpp"
#include "pmt/parser/generic_ast.hpp"
#include "pmt/parser/grammar/ast.hpp"
#include "pmt/parser/grammar/number.hpp"
#include "pmt/parser/grammar/string_literal.hpp"
#include "pmt/parser/primitives.hpp"

#include <algorithm>
#include <unordered_set>

namespace pmt::parser::grammar {
using namespace pmt::base;

namespace {}

auto GrammarData::construct_from_ast(GenericAst& ast_) -> GrammarData {
  GrammarData ret;

  initial_iteration(ret, ast_);
  add_reserved_terminal_accepts(ret);
  add_reserved_nonterminal_accepts(ret);
  sort_terminal_accepts_by_label(ret);
  sort_nonterminal_accepts_by_label(ret);
  create_eoi_terminal_definition(ret, ast_);
  check_terminal_uniqueness(ret);
  check_nonterminal_uniqueness(ret);
  check_start_nonterminal_label_defined(ret);
  final_iteration(ret, ast_);

  return ret;
}

auto GrammarData::lookup_terminal_label_by_index(size_t index_) const -> std::string {
  assert(index_ < _terminal_accepts.size() + _comment_open_definitions.size() + _comment_close_definitions.size() + 1);

  if (index_ >= _terminal_accepts.size() + _comment_open_definitions.size() + _comment_close_definitions.size()) {
    return GrammarData::TERMINAL_LABEL_WHITESPACE;
  }
  if (index_ >= _terminal_accepts.size() + _comment_open_definitions.size()) {
    index_ -= _terminal_accepts.size() + _comment_open_definitions.size();
    return GrammarData::TERMINAL_COMMENT_CLOSE_PREFIX + std::to_string(index_);
  }
  if (index_ >= _terminal_accepts.size()) {
    index_ -= _terminal_accepts.size();
    return GrammarData::TERMINAL_COMMENT_OPEN_PREFIX + std::to_string(index_);
  }

  return _terminal_accepts[index_]._label;
}

auto GrammarData::lookup_terminal_index_by_label(std::string_view label_) const -> size_t {
  assert(std::is_sorted(_terminal_accepts.begin(), _terminal_accepts.end(), [](auto const& lhs_, auto const& rhs_) { return lhs_._label < rhs_._label; }));

  if (label_.starts_with(GrammarData::TERMINAL_RESERVED_PREFIX_CH)) {
    return LookupIndexAnonymous;
  }

  auto const it = std::lower_bound(_terminal_accepts.begin(), _terminal_accepts.end(), label_, [](auto const& lhs_, auto const& rhs_) { return FetchLabelString{}(lhs_) < FetchLabelString{}(rhs_); });

  if (it == _terminal_accepts.end() && !label_.starts_with(GrammarData::TERMINAL_RESERVED_PREFIX_CH)) {
    throw std::runtime_error("Terminal not found: " + std::string(label_));
  }

  return std::distance(_terminal_accepts.begin(), it);
}

auto GrammarData::lookup_terminal_definition_by_index(size_t index_) const -> GenericAstPath {
  assert(index_ < _terminal_accepts.size() + _comment_open_definitions.size() + _comment_close_definitions.size() + 2);

  if (index_ >= _terminal_accepts.size() + _comment_open_definitions.size() + _comment_close_definitions.size() + 1) {
    return _linecount_definition;
  }
  if (index_ >= _terminal_accepts.size() + _comment_open_definitions.size() + _comment_close_definitions.size()) {
    return _whitespace_definition;
  }
  if (index_ >= _terminal_accepts.size() + _comment_open_definitions.size()) {
    index_ -= _terminal_accepts.size() + _comment_open_definitions.size();
    return _comment_close_definitions[index_];
  }
  if (index_ >= _terminal_accepts.size()) {
    index_ -= _terminal_accepts.size();
    return _comment_open_definitions[index_];
  }

  return _terminal_accepts[index_]._definition_path;
}

auto GrammarData::lookup_nonterminal_label_by_index(size_t index_) const -> std::string {
  return (index_ == _nonterminal_accepts.size()) ? TERMINAL_LABEL_START : (index_ < _nonterminal_accepts.size()) ? _nonterminal_accepts[index_]._label : "unknown";
}

auto GrammarData::lookup_nonterminal_index_by_label(std::string_view label_) const -> size_t {
  if (label_ == GrammarData::TERMINAL_LABEL_START) {
    return _nonterminal_accepts.size();
  }

  size_t const index = binary_find_index(_nonterminal_accepts.begin(), _nonterminal_accepts.end(), label_, [](auto const& lhs_, auto const& rhs_) { return FetchLabelString{}(lhs_) < FetchLabelString{}(rhs_); });

  if (index == _nonterminal_accepts.size() && !label_.starts_with(GrammarData::TERMINAL_RESERVED_PREFIX_CH)) {
    throw std::runtime_error("Nonterminal not found: " + std::string(label_));
  }

  return index;
}

auto GrammarData::lookup_nonterminal_definition_by_index(size_t index_) const -> GenericAstPath {
  return (index_ == _nonterminal_accepts.size()) ? _start_nonterminal_definition : _nonterminal_accepts[index_]._definition_path;
}

void GrammarData::initial_iteration(GrammarData& grammar_data_, GenericAst const& ast_) {
  for (size_t i = 0; i < ast_.get_children_size(); ++i) {
    GenericAst const& child = *ast_.get_child_at(i);
    switch (child.get_id()) {
      case Ast::NtGrammarProperty:
        initial_iteration_handle_grammar_property(grammar_data_, ast_, GenericAstPath(i));
        break;
      case Ast::NtTerminalProduction:
        initial_iteration_handle_terminal_production(grammar_data_, ast_, GenericAstPath(i));
        break;
      case Ast::NtNonterminalProduction:
        initial_iteration_handle_nonterminal_production(grammar_data_, ast_, GenericAstPath(i));
        break;
    }
  }
}

void GrammarData::initial_iteration_handle_grammar_property(GrammarData& grammar_data_, GenericAst const& ast_, GenericAstPath const& path_) {
  GenericAst const& grammar_property = *path_.resolve(ast_);
  GenericAst const& property_name = *grammar_property.get_child_at(0);
  GenericAstPath const property_value_position = path_.clone_push(1);
  switch (property_name.get_id()) {
    case Ast::TkGrammarPropertyStart:
      initial_iteration_handle_grammar_property_start(grammar_data_, ast_, property_value_position);
      break;
    case Ast::TkGrammarPropertyWhitespace:
      initial_iteration_handle_grammar_property_whitespace(grammar_data_, property_value_position);
      break;
    case Ast::TkGrammarPropertyComment:
      // in the case of comments, the pairs follow right after the property name
      initial_iteration_handle_grammar_property_comment(grammar_data_, ast_, path_);
      break;
    case Ast::TkGrammarPropertyNewline:
      initial_iteration_handle_grammar_property_newline(grammar_data_, property_value_position);
      break;
    default:
      pmt::unreachable();
  }
}

void GrammarData::initial_iteration_handle_grammar_property_start(GrammarData& grammar_data_, GenericAst const& ast_, GenericAstPath const& path_) {
  GenericAst const& value = *path_.resolve(ast_);
  assert(value.get_id() == Ast::TkNonterminalIdentifier);
  grammar_data_._start_nonterminal_definition = path_;
}

void GrammarData::initial_iteration_handle_grammar_property_whitespace(GrammarData& grammar_data_, GenericAstPath const& path_) {
  grammar_data_._whitespace_definition = path_;
}

void GrammarData::initial_iteration_handle_grammar_property_comment(GrammarData& grammar_data_, GenericAst const& ast_, GenericAstPath const& path_) {
  GenericAst const& terminal_definition_pair_list = *path_.resolve(ast_);
  for (size_t i = 1; i < terminal_definition_pair_list.get_children_size(); ++i) {
    grammar_data_._comment_open_definitions.push_back(path_.clone_push({i, 0}));
    grammar_data_._comment_close_definitions.push_back(path_.clone_push({i, 1}));
  }
}

void GrammarData::initial_iteration_handle_grammar_property_newline(GrammarData& grammar_data_, GenericAstPath const& path_) {
  grammar_data_._linecount_definition = path_;
}

void GrammarData::initial_iteration_handle_terminal_production(GrammarData& grammar_data_, GenericAst const& ast_, GenericAstPath const& path_) {
  GenericAst const& terminal_production = *path_.resolve(ast_);
  std::string terminal_label = terminal_production.get_child_at(0)->get_string();

  std::string terminal_id_name = GenericId::id_to_string(GenericId::IdDefault);
  GenericAstPath terminal_definition_position = path_.clone_push(0);

  for (size_t i = 1; i < terminal_production.get_children_size(); ++i) {
    GenericAst const& child = *terminal_production.get_child_at(i);
    switch (child.get_id()) {
      case Ast::NtTerminalParameter: {
        switch (child.get_child_at(0)->get_id()) {
          case Ast::TkKwParameterId:
            terminal_id_name = StringLiteral(*child.get_child_at(1)).get_value();
            break;
        }
      } break;
      case Ast::NtTerminalDefinition:
        terminal_definition_position.inplace_pop();
        terminal_definition_position.inplace_push(i);
        break;
    }
  }

  grammar_data_._terminal_accepts.emplace_back();
  grammar_data_._terminal_accepts.back()._label = std::move(terminal_label);
  grammar_data_._terminal_accepts.back()._id_name = std::move(terminal_id_name);
  grammar_data_._terminal_accepts.back()._accepted = false;

  grammar_data_._terminal_accepts.back()._definition_path = std::move(terminal_definition_position);
}

void GrammarData::initial_iteration_handle_nonterminal_production(GrammarData& grammar_data_, GenericAst const& ast_, GenericAstPath const& path_) {
  GenericAst const& nonterminal_production = *path_.resolve(ast_);
  std::string nonterminal_label = nonterminal_production.get_child_at(0)->get_string();

  std::string nonterminal_id_name = GenericId::id_to_string(GenericId::IdDefault);
  bool nonterminal_merge = MERGE_DEFAULT;
  bool nonterminal_unpack = UNPACK_DEFAULT;
  bool nonterminal_hide = HIDE_DEFAULT;
  GenericAstPath nonterminal_definition_position = path_.clone_push(0);

  for (size_t i = 1; i < nonterminal_production.get_children_size(); ++i) {
    GenericAst const& child = *nonterminal_production.get_child_at(i);
    switch (child.get_id()) {
      case Ast::NtNonterminalParameter: {
        switch (child.get_child_at(0)->get_id()) {
          case Ast::TkKwParameterId:
            nonterminal_id_name = StringLiteral(*child.get_child_at(1)).get_value();
            break;
          case Ast::TkKwParameterMerge:
            nonterminal_merge = child.get_child_at(1)->get_string() == "true";
            break;
          case Ast::TkKwParameterUnpack:
            nonterminal_unpack = child.get_child_at(1)->get_string() == "true";
            break;
          case Ast::TkKwParameterHide:
            nonterminal_hide = child.get_child_at(1)->get_string() == "true";
            break;
        }
      } break;
      case Ast::NtNonterminalDefinition:
        nonterminal_definition_position.inplace_pop();
        nonterminal_definition_position.inplace_push(i);
        break;
    }
  }

  grammar_data_._nonterminal_accepts.emplace_back();
  grammar_data_._nonterminal_accepts.back()._label = std::move(nonterminal_label);
  grammar_data_._nonterminal_accepts.back()._id_name = std::move(nonterminal_id_name);
  grammar_data_._nonterminal_accepts.back()._merge = nonterminal_merge;
  grammar_data_._nonterminal_accepts.back()._unpack = nonterminal_unpack;
  grammar_data_._nonterminal_accepts.back()._hide = nonterminal_hide;
  grammar_data_._nonterminal_accepts.back()._definition_path = std::move(nonterminal_definition_position);
}

void GrammarData::add_reserved_terminal_accepts(GrammarData& grammar_data_) {
  size_t const index_start = grammar_data_._terminal_accepts.size();
  grammar_data_._terminal_accepts.emplace_back();
  grammar_data_._terminal_accepts.back()._label = TERMINAL_LABEL_START;
  grammar_data_._terminal_accepts.back()._id_name = GenericId::id_to_string(GenericId::IdStart);

  size_t const index_eoi = grammar_data_._terminal_accepts.size();
  grammar_data_._terminal_accepts.emplace_back();
  grammar_data_._terminal_accepts.back()._label = LABEL_EOI;
  grammar_data_._terminal_accepts.back()._id_name = GenericId::id_to_string(GenericId::IdEoi);

  size_t const index_newline = grammar_data_._terminal_accepts.size();
  grammar_data_._terminal_accepts.emplace_back();
  grammar_data_._terminal_accepts.back()._label = TERMINAL_LABEL_LINECOUNT;
  grammar_data_._terminal_accepts.back()._id_name = GenericId::id_to_string(GenericId::IdNewline);
}

void GrammarData::add_reserved_nonterminal_accepts(GrammarData& grammar_data_) {
  grammar_data_._nonterminal_accepts.emplace_back();
  grammar_data_._nonterminal_accepts.back()._label = LABEL_EOI;
  grammar_data_._nonterminal_accepts.back()._id_name = GenericId::id_to_string(GenericId::IdEoi);
  grammar_data_._nonterminal_accepts.back()._hide = true;
}

void GrammarData::create_eoi_terminal_definition(GrammarData& grammar_data_, GenericAst& ast_) {
  GenericAst::UniqueHandle terminal_eoi_definition = GenericAst::construct(GenericAst::Tag::Children, Ast::NtTerminalDefinition);
  GenericAst::UniqueHandle terminal_eoi_definition_literal = GenericAst::construct(GenericAst::Tag::String, Ast::TkIntegerLiteral);
  terminal_eoi_definition_literal->set_string("10#" + std::to_string(SymbolValueEoi));
  terminal_eoi_definition->give_child_at_back(std::move(terminal_eoi_definition_literal));

  GenericAstPath const eoi_definition_path = GenericAstPath(ast_.get_children_size());
  ast_.give_child_at_back(std::move(terminal_eoi_definition));
  grammar_data_._terminal_accepts[grammar_data_.try_find_terminal_accept_index_by_label(GrammarData::LABEL_EOI)]._definition_path = eoi_definition_path;
}

void GrammarData::sort_terminal_accepts_by_label(GrammarData& grammar_data_) {
  std::ranges::sort(grammar_data_._terminal_accepts, [](TerminalAcceptData const& lhs_, TerminalAcceptData const& rhs_) { return lhs_._label < rhs_._label; });
}

void GrammarData::sort_nonterminal_accepts_by_label(GrammarData& grammar_data_) {
  std::ranges::sort(grammar_data_._nonterminal_accepts, [](NonterminalAcceptData const& lhs_, NonterminalAcceptData const& rhs_) { return lhs_._label < rhs_._label; });
}

void GrammarData::check_terminal_uniqueness(GrammarData& grammar_data_) {
  assert(std::ranges::is_sorted(grammar_data_._terminal_accepts, [](TerminalAcceptData const& lhs_, TerminalAcceptData const& rhs_) { return lhs_._label < rhs_._label; }));
  if (auto const itr = std::adjacent_find(grammar_data_._terminal_accepts.begin(), grammar_data_._terminal_accepts.end(), [](TerminalAcceptData const& lhs_, TerminalAcceptData const& rhs_) { return lhs_._label == rhs_._label; }); itr != grammar_data_._terminal_accepts.end()) {
    throw std::runtime_error("Terminal defined more than once: " + itr->_label);
  }
}

void GrammarData::check_nonterminal_uniqueness(GrammarData& grammar_data_) {
  assert(std::ranges::is_sorted(grammar_data_._nonterminal_accepts, [](NonterminalAcceptData const& lhs_, NonterminalAcceptData const& rhs_) { return lhs_._label < rhs_._label; }));
  if (auto const itr = std::adjacent_find(grammar_data_._nonterminal_accepts.begin(), grammar_data_._nonterminal_accepts.end(), [](NonterminalAcceptData const& lhs_, NonterminalAcceptData const& rhs_) { return lhs_._label == rhs_._label; }); itr != grammar_data_._nonterminal_accepts.end()) {
    throw std::runtime_error("Nonterminal defined more than once: " + itr->_label);
  }
}

void GrammarData::check_start_nonterminal_label_defined(GrammarData& grammar_data_) {
  if (grammar_data_._start_nonterminal_definition.empty()) {
    throw std::runtime_error("Start symbol not defined");
  }
}

void GrammarData::final_iteration(GrammarData& grammar_data_, GenericAst& ast_) {
  std::vector<GenericAstPath> pending;
  std::unordered_set<GenericAstPath> visited;

  auto const push_and_visit = [&pending, &visited](GenericAstPath const& path_) {
    if (visited.contains(path_)) {
      return;
    }

    pending.push_back(path_);
    visited.insert(path_);
  };

  push_and_visit(grammar_data_._nonterminal_accepts[grammar_data_.try_find_nonterminal_accept_index_by_label(grammar_data_._start_nonterminal_definition.resolve(ast_)->get_string())]._definition_path);

  while (!pending.empty()) {
    GenericAstPath const path_cur = pending.back();
    pending.pop_back();

    GenericId::IdType const id_cur = path_cur.resolve(ast_)->get_id();
    switch (id_cur) {
      case Ast::TkTerminalIdentifier: {
        std::string const& label = path_cur.resolve(ast_)->get_string();
        size_t const index = grammar_data_.try_find_terminal_accept_index_by_label(label);
        grammar_data_._terminal_accepts[index]._accepted = true;
      } break;
      case Ast::TkNonterminalIdentifier:
        push_and_visit(grammar_data_._nonterminal_accepts[grammar_data_.try_find_nonterminal_accept_index_by_label(path_cur.resolve(ast_)->get_string())]._definition_path);
        break;
      case Ast::NtNonterminalDefinition:
      case Ast::NtNonterminalExpression:
      case Ast::NtNonterminalChoices:
      case Ast::NtNonterminalSequence:
        for (size_t i = 0; i < path_cur.resolve(ast_)->get_children_size(); ++i) {
          push_and_visit(path_cur.clone_push(i));
        }
        break;
      case Ast::NtRepetitionExpression:
        push_and_visit(path_cur.clone_push(0));
        break;
      case Ast::NtTerminalHidden:
        push_and_visit(path_cur.clone_push(0));
        break;
      case Ast::TkStringLiteral:
      case Ast::TkIntegerLiteral: {
      } break;
    }
  }
}

auto GrammarData::try_find_terminal_accept_index_by_label(std::string const& label_) -> size_t {
  size_t const index = binary_find_index(_terminal_accepts.begin(), _terminal_accepts.end(), label_, [](auto const& lhs_, auto const& rhs_) { return FetchLabelString{}(lhs_) < FetchLabelString{}(rhs_); });
  if (index == _terminal_accepts.size()) {
    throw std::runtime_error("Terminal not found: " + label_);
  }
  return index;
}

auto GrammarData::try_find_nonterminal_accept_index_by_label(std::string const& label_) -> size_t {
  size_t const index = binary_find_index(_nonterminal_accepts.begin(), _nonterminal_accepts.end(), label_, [](auto const& lhs_, auto const& rhs_) { return FetchLabelString{}(lhs_) < FetchLabelString{}(rhs_); });
  if (index == _nonterminal_accepts.size()) {
    throw std::runtime_error("Nonterminal not found: " + label_);
  }
  return index;
}

}  // namespace pmt::parser::grammar