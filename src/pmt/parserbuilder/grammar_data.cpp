#include "pmt/parserbuilder/grammar_data.hpp"

#include "pmt/asserts.hpp"
#include "pmt/base/algo.hpp"
#include "pmt/parserbuilder/grm_ast.hpp"
#include "pmt/util/smrt/generic_ast.hpp"

#include <algorithm>
#include <unordered_set>

namespace pmt::parserbuilder {
using namespace pmt::base;
using namespace pmt::util::smrt;

auto GrammarData::construct_from_ast(GenericAst& ast_) -> GrammarData {
  GrammarData ret;

  initial_iteration(ret, ast_);
  add_reserved_terminal_accepts(ret);
  add_reserved_nonterminal_accepts(ret);
  sort_terminal_accepts_by_label(ret);
  sort_nonterminal_accepts_by_label(ret);
  check_terminal_uniqueness(ret);
  check_start_nonterminal_label_defined(ret);
  final_iteration(ret, ast_);

  return ret;
}

void GrammarData::initial_iteration(GrammarData& grammar_data_, pmt::util::smrt::GenericAst const& ast_) {
  for (size_t i = 0; i < ast_.get_children_size(); ++i) {
    GenericAst const& child = *ast_.get_child_at(i);
    switch (child.get_id()) {
      case GrmAst::NtGrammarProperty:
        initial_iteration_handle_grammar_property(grammar_data_, ast_, GenericAstPath(i));
        break;
      case GrmAst::NtTerminalProduction:
        initial_iteration_handle_terminal_production(grammar_data_, ast_, GenericAstPath(i));
        break;
      case GrmAst::NtNonterminalProduction:
        initial_iteration_handle_nonterminal_production(grammar_data_, ast_, GenericAstPath(i));
        break;
    }
  }
}

void GrammarData::initial_iteration_handle_grammar_property(GrammarData& grammar_data_, pmt::util::smrt::GenericAst const& ast_, pmt::util::smrt::GenericAstPath const& path_) {
  GenericAst const& grammar_property = *path_.resolve(ast_);
  GenericAst const& property_name = *grammar_property.get_child_at(0);
  GenericAstPath const property_value_position = path_.clone_push(1);
  switch (property_name.get_id()) {
    case GrmAst::TkGrammarPropertyCaseSensitive:
      initial_iteration_handle_grammar_property_case_sensitive(grammar_data_, ast_, property_value_position);
      break;
    case GrmAst::TkGrammarPropertyStart:
      initial_iteration_handle_grammar_property_start(grammar_data_, ast_, property_value_position);
      break;
    case GrmAst::TkGrammarPropertyWhitespace:
      initial_iteration_handle_grammar_property_whitespace(grammar_data_, property_value_position);
      break;
    case GrmAst::TkGrammarPropertyComment:
      initial_iteration_handle_grammar_property_comment(grammar_data_, ast_, property_value_position);
      break;
    case GrmAst::TkGrammarPropertyNewline:
      initial_iteration_handle_grammar_property_newline(grammar_data_, property_value_position);
      break;
    default:
      pmt::unreachable();
  }
}

void GrammarData::initial_iteration_handle_grammar_property_case_sensitive(GrammarData& grammar_data_, pmt::util::smrt::GenericAst const& ast_, pmt::util::smrt::GenericAstPath const& path_) {
 // noop
}

void GrammarData::initial_iteration_handle_grammar_property_start(GrammarData& grammar_data_, pmt::util::smrt::GenericAst const& ast_, pmt::util::smrt::GenericAstPath const& path_) {
  GenericAst const& value = *path_.resolve(ast_);
  assert(value.get_id() == GrmAst::TkNonterminalIdentifier);
  grammar_data_._start_nonterminal_definition = path_;
}

void GrammarData::initial_iteration_handle_grammar_property_whitespace(GrammarData& grammar_data_, pmt::util::smrt::GenericAstPath const& path_) {
  grammar_data_._whitespace_definition = path_;
}

void GrammarData::initial_iteration_handle_grammar_property_comment(GrammarData& grammar_data_, pmt::util::smrt::GenericAst const& ast_, pmt::util::smrt::GenericAstPath const& path_) {
  GenericAst const& terminal_definition_pair_list = *path_.resolve(ast_);
  for (size_t i = 0; i < terminal_definition_pair_list.get_children_size(); ++i) {
    grammar_data_._comment_open_definitions.push_back(path_.clone_push({i, 0}));
    grammar_data_._comment_close_definitions.push_back(path_.clone_push({i, 1}));
  }
}

void GrammarData::initial_iteration_handle_grammar_property_newline(GrammarData& grammar_data_, pmt::util::smrt::GenericAstPath const& path_) {
  grammar_data_._linecount_definition = path_;
}

void GrammarData::initial_iteration_handle_terminal_production(GrammarData& grammar_data_, pmt::util::smrt::GenericAst const& ast_, pmt::util::smrt::GenericAstPath const& path_) {
  GenericAst const& terminal_production = *path_.resolve(ast_);
  std::string terminal_label = terminal_production.get_child_at(0)->get_string();

  std::string terminal_id_name = GenericId::id_to_string(GenericId::IdDefault);
  GenericAstPath terminal_definition_position = path_.clone_push(0);

  for (size_t i = 1; i < terminal_production.get_children_size(); ++i) {
    GenericAst const& child = *terminal_production.get_child_at(i);
    switch (child.get_id()) {
      case GrmAst::NtTerminalParameter: {
        switch (child.get_child_at(0)->get_id()) {
          case GrmAst::TkKwParameterCaseSensitive:
            break;
          case GrmAst::TkKwParameterId:
            terminal_id_name = child.get_child_at(1)->get_string();
            break;
        }
      } break;
      case GrmAst::NtTerminalDefinition:
        terminal_definition_position.inplace_pop();
        terminal_definition_position.inplace_push(i);
        break;
    }
  }

  grammar_data_._terminal_accepts.emplace_back();
  grammar_data_._terminal_accepts.back()._label = std::move(terminal_label);
  grammar_data_._terminal_accepts.back()._id_name = std::move(terminal_id_name);
  grammar_data_._terminal_accepts.back()._hide = true;

  grammar_data_._terminal_accepts.back()._definition_path = std::move(terminal_definition_position);
}

void GrammarData::initial_iteration_handle_nonterminal_production(GrammarData& grammar_data_, pmt::util::smrt::GenericAst const& ast_, pmt::util::smrt::GenericAstPath const& path_) {
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
      case GrmAst::NtNonterminalParameter: {
        switch (child.get_child_at(0)->get_id()) {
          case GrmAst::TkKwParameterId:
            nonterminal_id_name = child.get_child_at(1)->get_string();
            break;
          case GrmAst::TkKwParameterMerge:
            nonterminal_merge = child.get_child_at(1)->get_string() == "true";
            break;
          case GrmAst::TkKwParameterUnpack:
            nonterminal_unpack = child.get_child_at(1)->get_string() == "true";
            break;
          case GrmAst::TkKwParameterHide:
            nonterminal_hide = child.get_child_at(1)->get_string() == "true";
            break;
        }
      } break;
      case GrmAst::NtNonterminalDefinition:
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

void GrammarData::check_start_nonterminal_label_defined(GrammarData& grammar_data_) {
  if (grammar_data_._start_nonterminal_definition.empty()) {
    throw std::runtime_error("Start symbol not defined");
  }
}

void GrammarData::final_iteration(GrammarData& grammar_data_, pmt::util::smrt::GenericAst& ast_) {
  std::vector<GenericAstPath> pending;
  std::unordered_set<GenericAstPath> visited;
  std::vector<std::string> terminals_direct_labels;
  std::vector<GenericAstPath> terminals_direct_definitions;

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

    switch (path_cur.resolve(ast_)->get_id()) {
      case GrmAst::TkTerminalIdentifier: {
        std::string const& label = path_cur.resolve(ast_)->get_string();
        size_t const index = grammar_data_.try_find_terminal_accept_index_by_label(label);
        grammar_data_._terminal_accepts[index]._hide = false;
      } break;
      case GrmAst::TkNonterminalIdentifier:
        push_and_visit(grammar_data_._nonterminal_accepts[grammar_data_.try_find_nonterminal_accept_index_by_label(path_cur.resolve(ast_)->get_string())]._definition_path);
        break;
      case GrmAst::NtTerminalDefinition:
      case GrmAst::NtNonterminalDefinition:
      case GrmAst::NtTerminalChoices:
      case GrmAst::NtTerminalSequence:
      case GrmAst::NtNonterminalChoices:
      case GrmAst::NtNonterminalSequence:
        for (size_t i = 0; i < path_cur.resolve(ast_)->get_children_size(); ++i) {
          push_and_visit(path_cur.clone_push(i));
        }
        break;
      case GrmAst::NtTerminalRepetition:
        push_and_visit(path_cur.clone_push(0));
        break;
      case GrmAst::NtNonterminalRepetition:
        push_and_visit(path_cur.clone_push(0));
        break;
      case GrmAst::TkStringLiteral:
      case GrmAst::TkIntegerLiteral: {
        std::string const terminal_direct_label = TERMINAL_DIRECT_PREFIX + std::to_string(terminals_direct_labels.size());
        terminals_direct_labels.push_back(terminal_direct_label);

        GenericAst::UniqueHandle terminal_direct_production = GenericAst::construct(GenericAst::Tag::Children, GrmAst::NtTerminalProduction);
        GenericAst::UniqueHandle terminal_direct_production_label = GenericAst::construct(GenericAst::Tag::String, GrmAst::TkTerminalIdentifier);
        terminal_direct_production_label->set_string(terminal_direct_label);
        terminal_direct_production->give_child_at_back(std::move(terminal_direct_production_label));
        GenericAst::UniqueHandle terminal_direct_production_definition = GenericAst::construct(GenericAst::Tag::Children, GrmAst::NtTerminalDefinition);
        terminal_direct_production_definition->give_child_at_back(GenericAst::clone(*path_cur.resolve(ast_)));
        terminal_direct_production->give_child_at_back(std::move(terminal_direct_production_definition));
        ast_.give_child_at_back(std::move(terminal_direct_production));

        GenericAstPath const path_direct_definition({ast_.get_children_size() - 1, 1});
        terminals_direct_definitions.push_back(path_direct_definition);
        path_cur.resolve(ast_)->set_string(terminal_direct_label);
        path_cur.resolve(ast_)->set_id(GrmAst::TkTerminalIdentifier);
      } break;
    }
  }

  while (!terminals_direct_labels.empty()) {
    size_t const index_direct = grammar_data_._terminal_accepts.size();

    grammar_data_._terminal_accepts.emplace_back();
    grammar_data_._terminal_accepts.back()._label = std::move(terminals_direct_labels.back());
    grammar_data_._terminal_accepts.back()._id_name = GenericId::id_to_string(GenericId::IdDefault);
    grammar_data_._terminal_accepts.back()._definition_path = std::move(terminals_direct_definitions.back());

    terminals_direct_labels.pop_back();
    terminals_direct_definitions.pop_back();
  }

  sort_terminal_accepts_by_label(grammar_data_);
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
    throw std::runtime_error("Rule not found: " + label_);
  }
  return index;
}

}  // namespace pmt::parserbuilder