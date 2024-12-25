#include "pmt/parserbuilder/parserbuilder.hpp"

#include "pmt/asserts.hpp"
#include "pmt/base/algo.hpp"
#include "pmt/base/dynamic_bitset.hpp"
#include "pmt/base/dynamic_bitset_converter.hpp"
#include "pmt/parserbuilder/grm_ast.hpp"
#include "pmt/parserbuilder/grm_lexer.hpp"
#include "pmt/parserbuilder/grm_parser.hpp"
#include "pmt/parserbuilder/terminal_definition_to_fa_part.hpp"
#include "pmt/util/parsect/graph_writer.hpp"
#include "pmt/util/parsert/generic_ast.hpp"
#include "pmt/util/parsert/generic_ast_printer.hpp"
#include "pmt/util/parsert/generic_tables_base.hpp"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <stack>
#include <utility>

namespace pmt::parserbuilder {
using namespace pmt::util::parsert;
using namespace pmt::util::parsect;
using namespace pmt::base;

void ParserBuilder::build(std::string_view input_grammar_path_) {
  std::ifstream input_grammar_stream(input_grammar_path_.data());
  std::string const input_grammar((std::istreambuf_iterator<char>(input_grammar_stream)), std::istreambuf_iterator<char>());

  GrmLexer lexer(input_grammar);

  Context context;
  context._ast = GrmParser::parse(lexer);

  GenericAstPrinter printer(GrmAst::id_to_string);
  printer.print(*context._ast, std::cout);

  step_01(context);
  step_02(context);
  step_03(context);
  step_04(context);
  step_05(context);
  step_06(context);
  step_07(context);
  step_08(context);
  step_09(context);
  step_10(context);
  step_11(context);
  step_12(context);

  // Debug print everything...
  std::cout << "Terminal names: ";
  for (auto const& name : context._terminal_names) {
    std::cout << name << " ";
  }
  std::cout << std::endl;

  std::cout << "Terminal id names: ";
  for (auto const& name : context._terminal_id_names) {
    std::cout << name << " ";
  }
  std::cout << std::endl;

  std::cout << "Terminal case sensitive values: " << DynamicBitsetConverter::to_string(context._terminal_case_sensitive_values) << std::endl;
  std::cout << "Terminal case sensitive present: " << DynamicBitsetConverter::to_string(context._terminal_case_sensitive_present) << std::endl;

  std::cout << "Start symbol: " << context._start_symbol << std::endl;
  std::cout << "Case sensitive: " << *context._case_sensitive << std::endl;
  std::cout << "Whitespace: ";
  for (auto const& symbol : context._whitespace) {
    std::cout << static_cast<int>(symbol) << " ";
  }
  std::cout << std::endl;

  std::cout << "Rule names: ";
  for (auto const& name : context._rule_names) {
    std::cout << name << " ";
  }
  std::cout << std::endl;

  std::cout << "Rule id names: ";
  for (auto const& name : context._rule_id_names) {
    std::cout << name << " ";
  }
  std::cout << std::endl;

  std::cout << "Rule merge values: " << DynamicBitsetConverter::to_string(context._rule_merge_values) << std::endl;
  std::cout << "Rule unpack values: " << DynamicBitsetConverter::to_string(context._rule_unpack_values) << std::endl;
  std::cout << "Rule hide values: " << DynamicBitsetConverter::to_string(context._rule_hide_values) << std::endl;

  write_dot(context, context._fa);
}

void ParserBuilder::step_01(Context& context_) {
  for (size_t i = 0; i < context_._ast->get_children_size(); ++i) {
    GenericAst const& child = *context_._ast->get_child_at(i);
    switch (child.get_id()) {
      case GrmAst::NtGrammarProperty:
        step_01_handle_grammar_property(context_, GenericAst::AstPositionConst{context_._ast.get(), i});
        break;
      case GrmAst::NtTerminalProduction:
        step_01_handle_terminal_production(context_, GenericAst::AstPositionConst{context_._ast.get(), i});
        break;
      case GrmAst::NtRuleProduction:
        step_01_handle_rule_production(context_, GenericAst::AstPositionConst{context_._ast.get(), i});
        break;
    }
  }
}

void ParserBuilder::step_01_handle_grammar_property(Context& context_, GenericAst::AstPositionConst position_) {
  GenericAst const& grammar_property = *position_.first->get_child_at(position_.second);
  GenericAst const& property_name = *grammar_property.get_child_at(0);
  GenericAst::AstPositionConst const property_value_position(&grammar_property, 1);
  switch (property_name.get_id()) {
    case GrmAst::TkGrammarPropertyCaseSensitive:
      step_01_handle_grammar_property_case_sensitive(context_, property_value_position);
      break;
    case GrmAst::TkGrammarPropertyStart:
      step_01_handle_grammar_property_start(context_, property_value_position);
      break;
    case GrmAst::TkGrammarPropertyWhitespace:
      step_01_handle_grammar_property_whitespace(context_, property_value_position);
      break;
    case GrmAst::TkGrammarPropertyComment:
      step_01_handle_grammar_property_comment(context_, property_value_position);
      break;
    default:
      pmt_unreachable();
  }
}

void ParserBuilder::step_01_handle_grammar_property_case_sensitive(Context& context_, GenericAst::AstPositionConst position_) {
  GenericAst const& value = *position_.first->get_child_at(position_.second);
  assert(value.get_id() == GrmAst::TkBooleanLiteral);
  context_._case_sensitive = value.get_string() == "true";
}

void ParserBuilder::step_01_handle_grammar_property_start(Context& context_, GenericAst::AstPositionConst position_) {
  GenericAst const& value = *position_.first->get_child_at(position_.second);
  assert(value.get_id() == GrmAst::TkRuleIdentifier);
  context_._start_symbol = value.get_string();
}

void ParserBuilder::step_01_handle_grammar_property_whitespace(Context& context_, GenericAst::AstPositionConst position_) {
  context_._whitespace_definition = position_;
}

void ParserBuilder::step_01_handle_grammar_property_comment(Context& context_, GenericAst::AstPositionConst position_) {
  GenericAst const& terminal_definition_pair_list = *position_.first->get_child_at(position_.second);
  for (size_t i = 0; i < terminal_definition_pair_list.get_children_size(); ++i) {
    GenericAst const& terminal_definition_pair = *terminal_definition_pair_list.get_child_at(i);
    context_._comment_open_definitions.push_back(GenericAst::AstPositionConst{&terminal_definition_pair, 0});
    context_._comment_close_definitions.push_back(GenericAst::AstPositionConst{&terminal_definition_pair, 1});
  }
}

void ParserBuilder::step_01_handle_terminal_production(Context& context_, GenericAst::AstPositionConst position_) {
  GenericAst const& terminal_production = *position_.first->get_child_at(position_.second);
  std::string terminal_name = terminal_production.get_child_at(0)->get_string();

  std::string terminal_id_name = GenericId::id_to_string(GenericId::IdDefault);
  std::optional<bool> terminal_case_sensitive;
  GenericAst::AstPositionConst terminal_definition_position(nullptr, 0);

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
        terminal_definition_position = GenericAst::AstPositionConst{&terminal_production, i};
        break;
    }
  }

  context_._terminal_names.push_back(terminal_name);
  context_._terminal_id_names.push_back(terminal_id_name);
  context_._terminal_case_sensitive_present.push_back(terminal_case_sensitive.has_value());
  context_._terminal_case_sensitive_values.push_back(terminal_case_sensitive.value_or(CASE_SENSITIVITY_DEFAULT));
  context_._terminal_definitions.push_back(terminal_definition_position);
}

void ParserBuilder::step_01_handle_rule_production(Context& context_, GenericAst::AstPositionConst position_) {
  GenericAst const& rule_production = *position_.first->get_child_at(position_.second);
  std::string rule_name = rule_production.get_child_at(0)->get_string();

  std::string rule_id_name = GenericId::id_to_string(GenericId::IdDefault);
  bool rule_merge = MERGE_DEFAULT;
  bool rule_unpack = UNPACK_DEFAULT;
  bool rule_hide = HIDE_DEFAULT;
  GenericAst::AstPositionConst rule_definition_position(nullptr, 0);

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
        rule_definition_position = GenericAst::AstPositionConst{&rule_production, i};
        break;
    }
  }

  context_._rule_names.push_back(rule_name);
  context_._rule_id_names.push_back(rule_id_name);
  context_._rule_merge_values.push_back(rule_merge);
  context_._rule_unpack_values.push_back(rule_unpack);
  context_._rule_hide_values.push_back(rule_hide);
  context_._rule_definitions.push_back(rule_definition_position);
}

void ParserBuilder::step_02(Context& context_) {
  context_._terminal_names.push_back("@eoi");
  context_._terminal_id_names.push_back(GenericId::id_to_string(GenericId::IdEoi));
  context_._terminal_case_sensitive_present.push_back(CASE_SENSITIVITY_DEFAULT);
  context_._terminal_case_sensitive_values.push_back(CASE_SENSITIVITY_DEFAULT);
  context_._terminal_definitions.push_back(GenericAst::AstPositionConst{nullptr, 0});

  context_._terminal_names.push_back("@start");
  context_._terminal_id_names.push_back(GenericId::id_to_string(GenericId::IdReserved1));
  context_._terminal_case_sensitive_present.push_back(CASE_SENSITIVITY_DEFAULT);
  context_._terminal_case_sensitive_values.push_back(CASE_SENSITIVITY_DEFAULT);
  context_._terminal_definitions.push_back(GenericAst::AstPositionConst{nullptr, 0});
}

void ParserBuilder::step_03(Context& context_) {
  // Sort the terminals by name
  std::vector<size_t> ordering;
  std::generate_n(std::back_inserter(ordering), context_._terminal_names.size(), [i = 0]() mutable { return i++; });
  std::ranges::sort(ordering, [&context_](size_t lhs_, size_t rhs_) { return context_._terminal_names[lhs_] < context_._terminal_names[rhs_]; });
  apply_permutation(context_._terminal_names.begin(), context_._terminal_names.end(), ordering.begin());
  apply_permutation(context_._terminal_id_names.begin(), context_._terminal_id_names.end(), ordering.begin());
  apply_permutation(context_._terminal_case_sensitive_present, ordering.begin());
  apply_permutation(context_._terminal_case_sensitive_values, ordering.begin());
  apply_permutation(context_._terminal_definitions.begin(), context_._terminal_definitions.end(), ordering.begin());

  // Sort the rules by name
  ordering.clear();
  std::generate_n(std::back_inserter(ordering), context_._rule_names.size(), [i = 0]() mutable { return i++; });
  std::ranges::sort(ordering, [&context_](size_t lhs_, size_t rhs_) { return context_._rule_names[lhs_] < context_._rule_names[rhs_]; });
  apply_permutation(context_._rule_names.begin(), context_._rule_names.end(), ordering.begin());
  apply_permutation(context_._rule_id_names.begin(), context_._rule_id_names.end(), ordering.begin());
  apply_permutation(context_._rule_merge_values, ordering.begin());
  apply_permutation(context_._rule_unpack_values, ordering.begin());
  apply_permutation(context_._rule_hide_values, ordering.begin());
  apply_permutation(context_._rule_definitions.begin(), context_._rule_definitions.end(), ordering.begin());
}

void ParserBuilder::step_04(Context& context_) {
  // Check that the terminal names are unique
  if (auto const itr = std::adjacent_find(context_._terminal_names.begin(), context_._terminal_names.end()); itr != context_._terminal_names.end()) {
    throw std::runtime_error("Terminal defined more than once: " + *itr);
  }

  // Check that the start symbol is defined
  if (context_._start_symbol.empty()) {
    throw std::runtime_error("Start symbol not defined");
  }

  // Set the case sensitivity
  context_._case_sensitive = context_._case_sensitive.value_or(CASE_SENSITIVITY_DEFAULT);

  for (size_t i = 0; i < context_._terminal_case_sensitive_values.size(); ++i) {
    if (context_._terminal_case_sensitive_present.get(i)) {
      continue;
    }

    context_._terminal_case_sensitive_values.set(i, *context_._case_sensitive);
  }
}

void ParserBuilder::step_05(Context& context_) {
  if (context_._whitespace_definition.first == nullptr) {
    return;
  }

  Fa fa_whitespace;
  FaPart fa_part_whitespace = TerminalDefinitionToFaPart::convert(fa_whitespace, "@whitespace", context_._whitespace_definition, context_._terminal_names, context_._terminal_definitions);
  Fa::StateNrType const state_nr_end = fa_whitespace.get_unused_state_nr();
  fa_whitespace._states[state_nr_end];
  fa_part_whitespace.connect_outgoing_transitions_to(state_nr_end, fa_whitespace);
  fa_whitespace.determinize();

  std::stack<std::pair<Fa::StateNrType, size_t>> stack;
  DynamicBitset visited(fa_whitespace._states.size(), false);

  auto const push_and_visit = [&stack, &visited](Fa::StateNrType state_nr_, size_t depth_) {
    if (visited.get(state_nr_)) {
      return;
    }

    stack.push({state_nr_, depth_});
    visited.set(state_nr_, true);
  };

  push_and_visit(0, 0);

  while (!stack.empty()) {
    auto const [state_nr_cur, depth_cur] = stack.top();
    stack.pop();

    if (depth_cur > 0 && !fa_whitespace._states.find(state_nr_cur)->second._transitions._symbol_transitions.empty()) {
      throw std::runtime_error("Whitespace definition too deep");
    }

    for (auto const& [symbol, state_nr_next] : fa_whitespace._states.find(state_nr_cur)->second._transitions._symbol_transitions) {
      context_._whitespace.insert(symbol);
      push_and_visit(state_nr_next, depth_cur + 1);
    }
  }
}

void ParserBuilder::step_06(Context& context_) {
  context_._terminal_accepts.resize(context_._terminal_names.size(), std::nullopt);

  {
    size_t const index_eoi = *pmt::base::binary_find_index(context_._terminal_names.begin(), context_._terminal_names.end(), "@eoi");
    context_._terminal_accepts[index_eoi] = context_._accepts.size();
    context_._accepts.push_back(index_eoi);
  }
  {
    size_t const index_start = *pmt::base::binary_find_index(context_._terminal_names.begin(), context_._terminal_names.end(), "@start");
    context_._terminal_accepts[index_start] = context_._accepts.size();
    context_._accepts.push_back(index_start);
  }

  std::stack<GenericAst::AstPositionConst> stack;
  std::unordered_set<GenericAst::AstPositionConst> visited;

  auto const push_and_visit = [&stack, &visited](GenericAst::AstPositionConst position_) {
    if (visited.contains(position_)) {
      return;
    }

    stack.push(position_);
    visited.insert(position_);
  };

  push_and_visit(context_.try_find_rule_definition(context_._start_symbol));

  while (!stack.empty()) {
    GenericAst::AstPositionConst const position_cur = stack.top();
    stack.pop();

    GenericAst const& cur = *position_cur.first->get_child_at(position_cur.second);

    switch (cur.get_id()) {
      case GrmAst::TkTerminalIdentifier: {
        std::string const& name = cur.get_string();
        std::optional<size_t> const index = base::binary_find_index(context_._terminal_names.begin(), context_._terminal_names.end(), name);
        if (!index.has_value()) {
          throw std::runtime_error("Terminal not found: " + name);
        }
        context_._terminal_accepts[*index] = context_._accepts.size();
        context_._accepts.push_back(*index);
      } break;
      case GrmAst::TkRuleIdentifier:
        push_and_visit(context_.try_find_rule_definition(cur.get_string()));
        break;
      case GrmAst::NtTerminalDefinition:
      case GrmAst::NtRuleDefinition:
      case GrmAst::NtTerminalChoices:
      case GrmAst::NtTerminalSequence:
      case GrmAst::NtRuleChoices:
      case GrmAst::NtRuleSequence:
        for (size_t i = 0; i < cur.get_children_size(); ++i) {
          push_and_visit(GenericAst::AstPositionConst{&cur, i});
        }
        break;
      case GrmAst::NtTerminalRepetition:
        push_and_visit(GenericAst::AstPositionConst{&cur, 0});
        break;
    }
  }
}

void ParserBuilder::step_07(Context& context_) {
  for (size_t i = 0; i < context_._comment_open_definitions.size(); ++i) {
    Fa fa_comment_open;
    FaPart fa_part_rule_open = TerminalDefinitionToFaPart::convert(fa_comment_open, "@comment_open_" + std::to_string(i), context_._comment_open_definitions[i], context_._terminal_names, context_._terminal_definitions);
    Fa::StateNrType const state_nr_end = fa_comment_open.get_unused_state_nr();
    fa_comment_open._states[state_nr_end]._accepts.resize(1, true);
    fa_part_rule_open.connect_outgoing_transitions_to(state_nr_end, fa_comment_open);
    fa_comment_open.determinize();
    context_._comment_open_fas.push_back(std::move(fa_comment_open));
  }

  for (size_t i = 0; i < context_._comment_close_definitions.size(); ++i) {
    Fa fa_comment_close;
    FaPart fa_part_rule_close = TerminalDefinitionToFaPart::convert(fa_comment_close, "@comment_close_" + std::to_string(i), context_._comment_close_definitions[i], context_._terminal_names, context_._terminal_definitions);
    Fa::StateNrType const state_nr_end = fa_comment_close.get_unused_state_nr();
    fa_comment_close._states[state_nr_end]._accepts.resize(1, true);
    fa_part_rule_close.connect_outgoing_transitions_to(state_nr_end, fa_comment_close);
    fa_comment_close.determinize();
    context_._comment_close_fas.push_back(std::move(fa_comment_close));
  }

  for (size_t i = 0; i < context_._terminal_accepts.size(); ++i) {
    if (!context_._terminal_accepts[i].has_value() || context_._terminal_names[i] == "@eoi" || context_._terminal_names[i] == "@start") {
      continue;
    }

    Fa fa_terminal;
    FaPart fa_part_terminal = TerminalDefinitionToFaPart::convert(fa_terminal, context_._terminal_id_names[i], context_._terminal_definitions[i], context_._terminal_names, context_._terminal_definitions);
    Fa::StateNrType const state_nr_end = fa_terminal.get_unused_state_nr();
    Fa::State& state_end = fa_terminal._states[state_nr_end];
    state_end._accepts.resize(context_._accepts.size(), false);
    state_end._accepts.set(*context_._terminal_accepts[i], true);
    fa_part_terminal.connect_outgoing_transitions_to(state_nr_end, fa_terminal);
    fa_terminal.determinize();
    context_._terminal_fas.push_back(std::move(fa_terminal));
  }
}

void ParserBuilder::step_08(Context& context_) {
  for (size_t i = 0; i < context_._comment_close_definitions.size(); ++i) {
    Fa& fa_comment_close = context_._comment_close_fas[i];

    std::vector<Fa::StateNrType> pending;
    std::unordered_set<Fa::StateNrType> visited;

    auto const push_and_visit = [&pending, &visited](Fa::StateNrType state_nr_) {
      if (visited.contains(state_nr_)) {
        return;
      }

      pending.push_back(state_nr_);
      visited.insert(state_nr_);
    };

    push_and_visit(0);

    while (!pending.empty()) {
      Fa::StateNrType const state_nr_cur = pending.back();
      pending.pop_back();

      Fa::State& state_cur = fa_comment_close._states.find(state_nr_cur)->second;

      if (state_cur._accepts.popcnt() == 0) {
        for (Fa::SymbolType symbol = 0; symbol < UCHAR_MAX; ++symbol) {
          auto const itr = state_cur._transitions._symbol_transitions.find(symbol);
          if (itr != state_cur._transitions._symbol_transitions.end()) {
            push_and_visit(itr->second);
          } else {
            state_cur._transitions._symbol_transitions.insert_or_assign(symbol, 0);
          }
        }
      }
    }
  }
}

void ParserBuilder::step_09(Context& context_) {
  Fa::StateNrType const state_nr_start = 0;
  context_._fa._states[state_nr_start];

  for (size_t i = 0; i < context_._comment_open_fas.size(); ++i) {
    Fa& fa_comment_open = context_._comment_open_fas[i];
    Fa::StateNrType const state_nr_comment_open = context_._fa._states.size();
    fa_comment_open.prune(0, state_nr_comment_open, true);

    Fa& fa_comment_close = context_._comment_close_fas[i];
    Fa::StateNrType const state_nr_comment_close = state_nr_comment_open + fa_comment_close._states.size();
    fa_comment_close.prune(0, state_nr_comment_close, true);

    context_._fa._states[state_nr_start]._transitions._epsilon_transitions.insert(state_nr_comment_open);

    for (auto& [state_nr, state] : fa_comment_open._states) {
      if (state._accepts.popcnt() == 0) {
        continue;
      }
      state._accepts.clear();
      state._transitions._epsilon_transitions.insert(state_nr_comment_close);
    }
    context_._fa._states.merge(fa_comment_open._states);

    for (auto& [state_nr, state] : fa_comment_close._states) {
      if (state._accepts.popcnt() == 0) {
        continue;
      }
      state._accepts.clear();
      state._transitions._epsilon_transitions.insert(state_nr_start);
    }
    context_._fa._states.merge(fa_comment_close._states);

    for (Fa::SymbolType const symbol : context_._whitespace) {
      if (context_._fa._states[state_nr_comment_open]._transitions._symbol_transitions.contains(symbol)) {
        throw std::runtime_error("Comment open conflicts with whitespace");
      }
      context_._fa._states[state_nr_comment_open]._transitions._symbol_transitions.insert_or_assign(symbol, state_nr_start);
    }
  }
}

void ParserBuilder::step_10(Context& context_) {
  Fa::StateNrType const state_nr_start = 0;
  Fa::State& state_start = context_._fa._states[state_nr_start];
  state_start._accepts.resize(context_._accepts.size(), false);
  std::optional<size_t> const index_start = base::binary_find_index(context_._terminal_names.begin(), context_._terminal_names.end(), "@start");
  state_start._accepts.set(*context_._terminal_accepts[*index_start], true);

  Fa::StateNrType const state_nr_eoi = context_._fa.get_unused_state_nr();
  Fa::State& state_eoi = context_._fa._states[state_nr_eoi];
  state_eoi._accepts.resize(context_._accepts.size(), false);
  std::optional<size_t> const index_eoi = base::binary_find_index(context_._terminal_names.begin(), context_._terminal_names.end(), "@eoi");
  state_eoi._accepts.set(*context_._terminal_accepts[*index_eoi], true);

  state_start._transitions._symbol_transitions.insert_or_assign(GenericTablesBase::SYMBOL_EOI, state_nr_eoi);
}

void ParserBuilder::step_11(Context& context_) {
  Fa::StateNrType const state_nr_start = 0;

  for (size_t i = 0; i < context_._terminal_fas.size(); ++i) {
    Fa& fa_terminal = context_._terminal_fas[i];
    Fa::StateNrType const state_nr_terminal = context_._fa.get_unused_state_nr();
    fa_terminal.prune(0, state_nr_terminal, true);
    context_._fa._states.merge(fa_terminal._states);

    context_._fa._states[state_nr_start]._transitions._epsilon_transitions.insert(state_nr_terminal);

    for (Fa::SymbolType const symbol : context_._whitespace) {
      if (context_._fa._states[state_nr_terminal]._transitions._symbol_transitions.contains(symbol)) {
        throw std::runtime_error("Terminal conflicts with whitespace");
      }
      context_._fa._states[state_nr_terminal]._transitions._symbol_transitions.insert_or_assign(symbol, state_nr_start);
    }
  }
}

void ParserBuilder::step_12(Context& context_) {
  context_._fa.determinize();
  context_._fa.minimize();

  pmt::base::DynamicBitset accepts_start = context_._fa._states.find(0)->second._accepts;
  size_t const index_start = *context_._terminal_accepts[*pmt::base::binary_find_index(context_._terminal_names.begin(), context_._terminal_names.end(), "@start")];
  accepts_start.set(index_start, false);

  if (accepts_start.any()) {
    static size_t const MAX_REPORTED = 8;
    std::unordered_set<size_t> const accepts_start_set = pmt::base::DynamicBitsetConverter::to_unordered_set(accepts_start);
    std::string msg;
    std::string delim;
    for (size_t i = 1; size_t const accept_nr : accepts_start_set) {
      msg += std::exchange(delim, ", ");
      if (i <= MAX_REPORTED) {
        msg += context_._terminal_names[context_._accepts[accept_nr]];
      } else {
        msg += "...";
        break;
      }
    }
    throw std::runtime_error("Initial state accepts terminal(s): " + msg);
  }

  size_t accepts_start_count = 0;
  for (auto const& [state_nr, state] : context_._fa._states) {
    if (state._accepts.size() == 0) {
      continue;
    }
    accepts_start_count += state._accepts.get(index_start) == true ? 1 : 0;
  }

  if (accepts_start_count != 1) {
    throw std::runtime_error("Error during building, there are " + std::to_string(accepts_start_count) + " starting states");
  }
}

void ParserBuilder::step_13(Context& context_) {
}

void ParserBuilder::write_dot(Context& context_, pmt::util::parsect::Fa const& fa_) {
  if (fa_._states.size() > DOT_FILE_MAX_STATES) {
    std::cerr << "Skipping dot file write, too many states\n";
    return;
  }

  std::ofstream file(DOT_FILE_PREFIX + std::to_string(context_._dot_file_count++) + ".dot");
  GraphWriter::write_dot(file, fa_, [&context_](size_t accepts_) { return accepts_to_label(context_, accepts_); });
}

auto ParserBuilder::accepts_to_label(Context& context_, size_t accept_idx_) -> std::string {
  return context_._terminal_names[context_._accepts[accept_idx_]];
}

auto ParserBuilder::Context::try_find_terminal_definition(std::string const& name_) -> pmt::util::parsert::GenericAst::AstPositionConst {
  std::optional<size_t> const index = base::binary_find_index(_terminal_names.begin(), _terminal_names.end(), name_);
  if (!index.has_value()) {
    throw std::runtime_error("Terminal not found: " + name_);
  }

  return _terminal_definitions[*index];
}

auto ParserBuilder::Context::try_find_rule_definition(std::string const& name_) -> pmt::util::parsert::GenericAst::AstPositionConst {
  std::optional<size_t> const index = base::binary_find_index(_rule_names.begin(), _rule_names.end(), name_);
  if (!index.has_value()) {
    throw std::runtime_error("Rule not found: " + name_);
  }

  return _rule_definitions[*index];
}

}  // namespace pmt::parserbuilder
