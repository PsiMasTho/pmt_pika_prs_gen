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
  add_start_and_eoi_terminals(ret);
  sort_terminals_by_name(ret);
  sort_rules_by_name(ret);
  check_terminal_uniqueness(ret);
  check_start_rule_name_defined(ret);
  final_iteration(ret, ast_);

  return ret;
}

void GrammarData::initial_iteration(GrammarData& grammar_data_, pmt::util::smrt::GenericAst const& ast_) {
  InitialIterationContext context_;

  for (size_t i = 0; i < ast_.get_children_size(); ++i) {
    GenericAst const& child = *ast_.get_child_at(i);
    switch (child.get_id()) {
      case GrmAst::NtGrammarProperty:
        initial_iteration_handle_grammar_property(grammar_data_, ast_, GenericAstPath(i), context_);
        break;
      case GrmAst::NtTerminalProduction:
        initial_iteration_handle_terminal_production(grammar_data_, ast_, GenericAstPath(i), context_);
        break;
      case GrmAst::NtRuleProduction:
        initial_iteration_handle_rule_production(grammar_data_, ast_, GenericAstPath(i));
        break;
    }
  }

  // Set the case sensitivity
  if (!context_._grammar_property_case_sensitive_present) {
    grammar_data_._case_sensitive = CASE_SENSITIVITY_DEFAULT;
  }

  for (size_t i = 0; i < context_._terminal_case_sensitive_present.size(); ++i) {
    if (context_._terminal_case_sensitive_present.get(i)) {
      continue;
    }

    grammar_data_._terminals[i]._case_sensitive = grammar_data_._case_sensitive;
  }
}

void GrammarData::initial_iteration_handle_grammar_property(GrammarData& grammar_data_, pmt::util::smrt::GenericAst const& ast_, pmt::util::smrt::GenericAstPath const& path_, InitialIterationContext& context_) {
  GenericAst const& grammar_property = *path_.resolve(ast_);
  GenericAst const& property_name = *grammar_property.get_child_at(0);
  GenericAstPath const property_value_position = path_.clone_push(1);
  switch (property_name.get_id()) {
    case GrmAst::TkGrammarPropertyCaseSensitive:
      initial_iteration_handle_grammar_property_case_sensitive(grammar_data_, ast_, property_value_position, context_);
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
    default:
      pmt::unreachable();
  }
}

void GrammarData::initial_iteration_handle_grammar_property_case_sensitive(GrammarData& grammar_data_, pmt::util::smrt::GenericAst const& ast_, pmt::util::smrt::GenericAstPath const& path_, InitialIterationContext& context_) {
  GenericAst const& value = *path_.resolve(ast_);
  assert(value.get_id() == GrmAst::TkBooleanLiteral);
  grammar_data_._case_sensitive = value.get_string() == "true";
  context_._grammar_property_case_sensitive_present = true;
}

void GrammarData::initial_iteration_handle_grammar_property_start(GrammarData& grammar_data_, pmt::util::smrt::GenericAst const& ast_, pmt::util::smrt::GenericAstPath const& path_) {
  GenericAst const& value = *path_.resolve(ast_);
  assert(value.get_id() == GrmAst::TkRuleIdentifier);
  grammar_data_._start_rule_name = value.get_string();
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

void GrammarData::initial_iteration_handle_terminal_production(GrammarData& grammar_data_, pmt::util::smrt::GenericAst const& ast_, pmt::util::smrt::GenericAstPath const& path_, InitialIterationContext& context_) {
  GenericAst const& terminal_production = *path_.resolve(ast_);
  std::string terminal_name = terminal_production.get_child_at(0)->get_string();

  std::string terminal_id_name = GenericId::id_to_string(GenericId::IdDefault);
  std::optional<bool> terminal_case_sensitive;
  GenericAstPath terminal_definition_position = path_.clone_push(0);

  for (size_t i = 1; i < terminal_production.get_children_size(); ++i) {
    GenericAst const& child = *terminal_production.get_child_at(i);
    switch (child.get_id()) {
      case GrmAst::NtTerminalParameter: {
        switch (child.get_child_at(0)->get_id()) {
          case GrmAst::TkKwParameterCaseSensitive:
            terminal_case_sensitive = child.get_child_at(1)->get_string() == "true";
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

  grammar_data_._terminals.emplace_back();
  grammar_data_._terminals.back()._name = std::move(terminal_name);
  grammar_data_._terminals.back()._id_name = std::move(terminal_id_name);

  context_._terminal_case_sensitive_present.push_back(terminal_case_sensitive.has_value());
  if (context_._terminal_case_sensitive_present.back()) {
    grammar_data_._terminals.back()._case_sensitive = terminal_case_sensitive.value();
  }

  grammar_data_._terminals.back()._definition_path = std::move(terminal_definition_position);
}

void GrammarData::initial_iteration_handle_rule_production(GrammarData& grammar_data_, pmt::util::smrt::GenericAst const& ast_, pmt::util::smrt::GenericAstPath const& path_) {
  GenericAst const& rule_production = *path_.resolve(ast_);
  std::string rule_name = rule_production.get_child_at(0)->get_string();

  std::string rule_id_name = GenericId::id_to_string(GenericId::IdDefault);
  bool rule_merge = MERGE_DEFAULT;
  bool rule_unpack = UNPACK_DEFAULT;
  bool rule_hide = HIDE_DEFAULT;
  GenericAstPath rule_definition_position = path_.clone_push(0);

  for (size_t i = 1; i < rule_production.get_children_size(); ++i) {
    GenericAst const& child = *rule_production.get_child_at(i);
    switch (child.get_id()) {
      case GrmAst::NtRuleParameter: {
        switch (child.get_child_at(0)->get_id()) {
          case GrmAst::TkKwParameterId:
            rule_id_name = child.get_child_at(1)->get_string();
            break;
          case GrmAst::TkKwParameterMerge:
            rule_merge = child.get_child_at(1)->get_string() == "true";
            break;
          case GrmAst::TkKwParameterUnpack:
            rule_unpack = child.get_child_at(1)->get_string() == "true";
            break;
          case GrmAst::TkKwParameterHide:
            rule_hide = child.get_child_at(1)->get_string() == "true";
            break;
        }
      } break;
      case GrmAst::NtRuleDefinition:
        rule_definition_position.inplace_pop();
        rule_definition_position.inplace_push(i);
        break;
    }
  }

  grammar_data_._rules.emplace_back();
  grammar_data_._rules.back()._name = std::move(rule_name);
  grammar_data_._rules.back()._id_name = std::move(rule_id_name);
  grammar_data_._rules.back()._merge = rule_merge;
  grammar_data_._rules.back()._unpack = rule_unpack;
  grammar_data_._rules.back()._hide = rule_hide;
  grammar_data_._rules.back()._definition_path = std::move(rule_definition_position);
}

void GrammarData::add_start_and_eoi_terminals(GrammarData& grammar_data_) {
  size_t const index_start = grammar_data_._terminals.size();
  grammar_data_._terminals.emplace_back();
  grammar_data_._terminals.back()._name = TERMINAL_NAME_START;
  grammar_data_._terminals.back()._id_name = GenericId::id_to_string(GenericId::IdStart);
  grammar_data_._terminals.back()._accepts = grammar_data_._accepts_reverse.size();
  grammar_data_._accepts_reverse.push_back(index_start);

  size_t const index_eoi = grammar_data_._terminals.size();
  grammar_data_._terminals.emplace_back();
  grammar_data_._terminals.back()._name = TERMINAL_NAME_EOI;
  grammar_data_._terminals.back()._id_name = GenericId::id_to_string(GenericId::IdEoi);
  grammar_data_._terminals.back()._accepts = grammar_data_._accepts_reverse.size();
  grammar_data_._accepts_reverse.push_back(index_eoi);
}

void GrammarData::sort_terminals_by_name(GrammarData& grammar_data_) {
  std::vector<size_t> ordering;
  std::generate_n(std::back_inserter(ordering), grammar_data_._terminals.size(), [i = 0]() mutable { return i++; });
  std::ranges::sort(ordering, [&grammar_data_](size_t lhs_, size_t rhs_) { return grammar_data_._terminals[lhs_]._name < grammar_data_._terminals[rhs_]._name; });

  apply_permutation(grammar_data_._terminals.begin(), grammar_data_._terminals.end(), ordering.begin());

  std::vector<size_t> const ordering_inverse = inverse_permutation(ordering.begin(), ordering.end());

  std::transform(grammar_data_._accepts_reverse.begin(), grammar_data_._accepts_reverse.end(), grammar_data_._accepts_reverse.begin(), [&ordering_inverse](size_t i_) { return ordering_inverse[i_]; });
}

void GrammarData::sort_rules_by_name(GrammarData& grammar_data_) {
  std::ranges::sort(grammar_data_._rules, [](RuleData const& lhs_, RuleData const& rhs_) { return lhs_._name < rhs_._name; });
}

void GrammarData::check_terminal_uniqueness(GrammarData& grammar_data_) {
  assert(std::ranges::is_sorted(grammar_data_._terminals, [](TerminalData const& lhs_, TerminalData const& rhs_) { return lhs_._name < rhs_._name; }));
  if (auto const itr = std::adjacent_find(grammar_data_._terminals.begin(), grammar_data_._terminals.end(), [](TerminalData const& lhs_, TerminalData const& rhs_) { return lhs_._name == rhs_._name; }); itr != grammar_data_._terminals.end()) {
    throw std::runtime_error("Terminal defined more than once: " + itr->_name);
  }
}

void GrammarData::check_start_rule_name_defined(GrammarData& grammar_data_) {
  if (grammar_data_._start_rule_name.empty()) {
    throw std::runtime_error("Start symbol not defined");
  }
}

void GrammarData::final_iteration(GrammarData& grammar_data_, pmt::util::smrt::GenericAst& ast_) {
  std::vector<GenericAstPath> pending;
  std::unordered_set<GenericAstPath> visited;
  std::vector<std::string> terminals_direct_names;
  std::vector<GenericAstPath> terminals_direct_definitions;

  auto const push_and_visit = [&pending, &visited](GenericAstPath const& path_) {
    if (visited.contains(path_)) {
      return;
    }

    pending.push_back(path_);
    visited.insert(path_);
  };

  push_and_visit(grammar_data_._rules[grammar_data_.try_find_rule_index_by_name(grammar_data_._start_rule_name)]._definition_path);

  while (!pending.empty()) {
    GenericAstPath const path_cur = pending.back();
    pending.pop_back();

    switch (path_cur.resolve(ast_)->get_id()) {
      case GrmAst::TkTerminalIdentifier: {
        std::string const& name = path_cur.resolve(ast_)->get_string();
        size_t const index = grammar_data_.try_find_terminal_index_by_name(name);
        grammar_data_._terminals[index]._accepts = grammar_data_._accepts_reverse.size();
        grammar_data_._accepts_reverse.push_back(index);
      } break;
      case GrmAst::TkRuleIdentifier:
        push_and_visit(grammar_data_._rules[grammar_data_.try_find_rule_index_by_name(path_cur.resolve(ast_)->get_string())]._definition_path);
        break;
      case GrmAst::NtTerminalDefinition:
      case GrmAst::NtRuleDefinition:
      case GrmAst::NtTerminalChoices:
      case GrmAst::NtTerminalSequence:
      case GrmAst::NtRuleChoices:
      case GrmAst::NtRuleSequence:
        for (size_t i = 0; i < path_cur.resolve(ast_)->get_children_size(); ++i) {
          push_and_visit(path_cur.clone_push(i));
        }
        break;
      case GrmAst::NtTerminalRepetition:
        push_and_visit(path_cur.clone_push(0));
        break;
      case GrmAst::TkStringLiteral:
      case GrmAst::TkIntegerLiteral: {
        std::string const terminal_direct_name = TERMINAL_DIRECT_PREFIX + std::to_string(terminals_direct_names.size());
        terminals_direct_names.push_back(terminal_direct_name);

        GenericAst::UniqueHandle terminal_direct_production = GenericAst::construct(GenericAst::Tag::Children, GrmAst::NtTerminalProduction);
        GenericAst::UniqueHandle terminal_direct_production_name = GenericAst::construct(GenericAst::Tag::String, GrmAst::TkTerminalIdentifier);
        terminal_direct_production_name->set_string(terminal_direct_name);
        terminal_direct_production->give_child_at_back(std::move(terminal_direct_production_name));
        GenericAst::UniqueHandle terminal_direct_production_definition = GenericAst::construct(GenericAst::Tag::Children, GrmAst::NtTerminalDefinition);
        terminal_direct_production_definition->give_child_at_back(GenericAst::clone(*path_cur.resolve(ast_)));
        terminal_direct_production->give_child_at_back(std::move(terminal_direct_production_definition));
        ast_.give_child_at_back(std::move(terminal_direct_production));

        GenericAstPath const path_direct_definition({ast_.get_children_size() - 1, 1});
        terminals_direct_definitions.push_back(path_direct_definition);
        path_cur.resolve(ast_)->set_string(terminal_direct_name);
        path_cur.resolve(ast_)->set_id(GrmAst::TkTerminalIdentifier);
      } break;
    }
  }

  while (!terminals_direct_names.empty()) {
    size_t const index_direct = grammar_data_._terminals.size();

    grammar_data_._terminals.emplace_back();
    grammar_data_._terminals.back()._name = std::move(terminals_direct_names.back());
    grammar_data_._terminals.back()._id_name = GenericId::id_to_string(GenericId::IdDefault);
    grammar_data_._terminals.back()._case_sensitive = grammar_data_._case_sensitive;
    grammar_data_._terminals.back()._definition_path = std::move(terminals_direct_definitions.back());
    grammar_data_._terminals.back()._accepts = grammar_data_._accepts_reverse.size();
    grammar_data_._accepts_reverse.push_back(index_direct);

    terminals_direct_names.pop_back();
    terminals_direct_definitions.pop_back();
  }

  sort_terminals_by_name(grammar_data_);
}

auto GrammarData::try_find_terminal_index_by_name(std::string const& name_) -> size_t {
  size_t const index = binary_find_index(_terminals.begin(), _terminals.end(), name_, [](auto const& lhs_, auto const& rhs_) { return FetchNameString{}(lhs_) < FetchNameString{}(rhs_); });
  if (index == _terminals.size()) {
    throw std::runtime_error("Terminal not found: " + name_);
  }
  return index;
}

auto GrammarData::try_find_rule_index_by_name(std::string const& name_) -> size_t {
  size_t const index = binary_find_index(_rules.begin(), _rules.end(), name_, [](auto const& lhs_, auto const& rhs_) { return FetchNameString{}(lhs_) < FetchNameString{}(rhs_); });
  if (index == _rules.size()) {
    throw std::runtime_error("Rule not found: " + name_);
  }
  return index;
}

}  // namespace pmt::parserbuilder