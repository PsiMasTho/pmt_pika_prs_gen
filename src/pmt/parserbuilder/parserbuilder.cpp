#include "pmt/parserbuilder/parserbuilder.hpp"

#include "pmt/asserts.hpp"
#include "pmt/base/algo.hpp"
#include "pmt/base/bitset.hpp"
#include "pmt/base/bitset_converter.hpp"
#include "pmt/base/interval_set.hpp"
#include "pmt/parserbuilder/definition_to_state_machine_part.hpp"
#include "pmt/parserbuilder/grm_ast.hpp"
#include "pmt/parserbuilder/grm_lexer.hpp"
#include "pmt/parserbuilder/grm_parser.hpp"
#include "pmt/util/smct/graph_writer.hpp"
#include "pmt/util/smct/state_machine_determinizer.hpp"
#include "pmt/util/smct/state_machine_minimizer.hpp"
#include "pmt/util/smct/state_machine_part.hpp"
#include "pmt/util/smct/state_machine_pruner.hpp"
#include "pmt/util/smrt/generic_ast.hpp"
#include "pmt/util/smrt/generic_ast_printer.hpp"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <stack>
#include <utility>

namespace pmt::parserbuilder {
using namespace pmt::util::smrt;
using namespace pmt::util::smct;
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

  std::cout << "Terminal case sensitive values: " << BitsetConverter::to_string(context._terminal_case_sensitive_values) << std::endl;
  std::cout << "Terminal case sensitive present: " << BitsetConverter::to_string(context._terminal_case_sensitive_present) << std::endl;

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

  std::cout << "Rule merge values: " << BitsetConverter::to_string(context._rule_merge_values) << std::endl;
  std::cout << "Rule unpack values: " << BitsetConverter::to_string(context._rule_unpack_values) << std::endl;
  std::cout << "Rule hide values: " << BitsetConverter::to_string(context._rule_hide_values) << std::endl;

  std::cout << "Ast after: " << std::endl;
  printer.print(*context._ast, std::cout);

  write_dot(context, context._fsm);
}

void ParserBuilder::step_01(Context& context_) {
  for (size_t i = 0; i < context_._ast->get_children_size(); ++i) {
    GenericAst const& child = *context_._ast->get_child_at(i);
    switch (child.get_id()) {
      case GrmAst::NtGrammarProperty:
        step_01_handle_grammar_property(context_, {i});
        break;
      case GrmAst::NtTerminalProduction:
        step_01_handle_terminal_production(context_, {i});
        break;
      case GrmAst::NtRuleProduction:
        step_01_handle_rule_production(context_, {i});
        break;
    }
  }
}

void ParserBuilder::step_01_handle_grammar_property(Context& context_, GenericAstPath const& path_) {
  GenericAst const& grammar_property = *path_.resolve(*context_._ast);
  GenericAst const& property_name = *grammar_property.get_child_at(0);
  GenericAstPath const property_value_position = path_.clone_push(1);
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
      pmt::unreachable();
  }
}

void ParserBuilder::step_01_handle_grammar_property_case_sensitive(Context& context_, GenericAstPath const& path_) {
  GenericAst const& value = *path_.resolve(*context_._ast);
  assert(value.get_id() == GrmAst::TkBooleanLiteral);
  context_._case_sensitive = value.get_string() == "true";
}

void ParserBuilder::step_01_handle_grammar_property_start(Context& context_, GenericAstPath const& path_) {
  GenericAst const& value = *path_.resolve(*context_._ast);
  assert(value.get_id() == GrmAst::TkRuleIdentifier);
  context_._start_symbol = value.get_string();
}

void ParserBuilder::step_01_handle_grammar_property_whitespace(Context& context_, GenericAstPath const& path_) {
  context_._whitespace_definition = path_;
}

void ParserBuilder::step_01_handle_grammar_property_comment(Context& context_, GenericAstPath const& path_) {
  GenericAst const& terminal_definition_pair_list = *path_.resolve(*context_._ast);
  for (size_t i = 0; i < terminal_definition_pair_list.get_children_size(); ++i) {
    context_._comment_open_definitions.push_back(path_.clone_push({i, 0}));
    context_._comment_close_definitions.push_back(path_.clone_push({i, 1}));
  }
}

void ParserBuilder::step_01_handle_terminal_production(Context& context_, GenericAstPath const& path_) {
  GenericAst const& terminal_production = *path_.resolve(*context_._ast);
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

  context_._terminal_names.push_back(terminal_name);
  context_._terminal_id_names.push_back(terminal_id_name);
  context_._terminal_case_sensitive_present.push_back(terminal_case_sensitive.has_value());
  context_._terminal_case_sensitive_values.push_back(terminal_case_sensitive.value_or(CASE_SENSITIVITY_DEFAULT));
  context_._terminal_definitions.push_back(terminal_definition_position);
}

void ParserBuilder::step_01_handle_rule_production(Context& context_, GenericAstPath const& path_) {
  GenericAst const& rule_production = *path_.resolve(*context_._ast);
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

  context_._rule_names.push_back(rule_name);
  context_._rule_id_names.push_back(rule_id_name);
  context_._rule_merge_values.push_back(rule_merge);
  context_._rule_unpack_values.push_back(rule_unpack);
  context_._rule_hide_values.push_back(rule_hide);
  context_._rule_definitions.push_back(rule_definition_position);
}

void ParserBuilder::step_02(Context& context_) {
  context_._terminal_accepts.resize(context_._terminal_names.size(), std::nullopt);

  size_t const index_start = context_._terminal_names.size();
  context_._terminal_names.push_back("@start");
  context_._terminal_id_names.push_back(GenericId::id_to_string(GenericId::IdReserved1));
  context_._terminal_case_sensitive_present.push_back(CASE_SENSITIVITY_DEFAULT);
  context_._terminal_case_sensitive_values.push_back(CASE_SENSITIVITY_DEFAULT);
  context_._terminal_definitions.emplace_back();
  context_._terminal_accepts.push_back(context_._accepts.size());
  context_._accepts.push_back(index_start);

  size_t const index_eoi = context_._terminal_names.size();
  context_._terminal_names.push_back("@eoi");
  context_._terminal_id_names.push_back(GenericId::id_to_string(GenericId::IdEoi));
  context_._terminal_case_sensitive_present.push_back(CASE_SENSITIVITY_DEFAULT);
  context_._terminal_case_sensitive_values.push_back(CASE_SENSITIVITY_DEFAULT);
  context_._terminal_definitions.emplace_back();
  context_._terminal_accepts.push_back(context_._accepts.size());
  context_._accepts.push_back(index_eoi);

  sort_terminals_by_name(context_);
  sort_rules_by_name(context_);
}

void ParserBuilder::step_03(Context& context_) {
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

void ParserBuilder::step_04(Context& context_) {
  std::stack<GenericAstPath> stack;
  std::unordered_set<GenericAstPath> visited;
  std::vector<std::string> terminals_direct_names;
  std::vector<GenericAstPath> terminals_direct_definitions;

  auto const push_and_visit = [&stack, &visited](GenericAstPath const& path_) {
    if (visited.contains(path_)) {
      return;
    }

    stack.push(path_);
    visited.insert(path_);
  };

  push_and_visit(context_.try_find_rule_definition(context_._start_symbol));

  while (!stack.empty()) {
    GenericAstPath const path_cur = stack.top();
    stack.pop();

    switch (path_cur.resolve(*context_._ast)->get_id()) {
      case GrmAst::TkTerminalIdentifier: {
        std::string const& name = path_cur.resolve(*context_._ast)->get_string();
        std::optional<size_t> const index = base::binary_find_index(context_._terminal_names.begin(), context_._terminal_names.end(), name);
        if (!index.has_value()) {
          throw std::runtime_error("Terminal not found: " + name);
        }
        context_._terminal_accepts[*index] = context_._accepts.size();
        context_._accepts.push_back(*index);
      } break;
      case GrmAst::TkRuleIdentifier:
        push_and_visit(context_.try_find_rule_definition(path_cur.resolve(*context_._ast)->get_string()));
        break;
      case GrmAst::NtTerminalDefinition:
      case GrmAst::NtRuleDefinition:
      case GrmAst::NtTerminalChoices:
      case GrmAst::NtTerminalSequence:
      case GrmAst::NtRuleChoices:
      case GrmAst::NtRuleSequence:
        for (size_t i = 0; i < path_cur.resolve(*context_._ast)->get_children_size(); ++i) {
          push_and_visit(path_cur.clone_push(i));
        }
        break;
      case GrmAst::NtTerminalRepetition:
        push_and_visit(path_cur.clone_push(0));
        break;
      case GrmAst::TkStringLiteral:
      case GrmAst::TkIntegerLiteral: {
        std::string const terminal_direct_name = "@direct_" + std::to_string(terminals_direct_names.size());
        terminals_direct_names.push_back(terminal_direct_name);

        GenericAst::UniqueHandle terminal_direct_production = GenericAst::construct(GenericAst::Tag::Children, GrmAst::NtTerminalProduction);
        GenericAst::UniqueHandle terminal_direct_production_name = GenericAst::construct(GenericAst::Tag::String, GrmAst::TkTerminalIdentifier);
        terminal_direct_production_name->set_string(terminal_direct_name);
        terminal_direct_production->give_child_at_back(std::move(terminal_direct_production_name));
        GenericAst::UniqueHandle terminal_direct_production_definition = GenericAst::construct(GenericAst::Tag::Children, GrmAst::NtTerminalDefinition);
        terminal_direct_production_definition->give_child_at_back(GenericAst::clone(*path_cur.resolve(*context_._ast)));
        terminal_direct_production->give_child_at_back(std::move(terminal_direct_production_definition));
        context_._ast->give_child_at_back(std::move(terminal_direct_production));

        GenericAstPath const path_direct_definition({context_._ast->get_children_size() - 1, 1});
        terminals_direct_definitions.push_back(path_direct_definition);
        path_cur.resolve(*context_._ast)->set_string(terminal_direct_name);
        path_cur.resolve(*context_._ast)->set_id(GrmAst::TkTerminalIdentifier);
      } break;
    }
  }

  while (!terminals_direct_names.empty()) {
    size_t const index_direct = context_._terminal_names.size();
    context_._terminal_names.push_back(terminals_direct_names.back());
    context_._terminal_id_names.push_back(GenericId::id_to_string(GenericId::IdDefault));
    context_._terminal_case_sensitive_present.push_back(true);
    context_._terminal_case_sensitive_values.push_back(true);
    context_._terminal_definitions.push_back(terminals_direct_definitions.back());
    context_._terminal_accepts.push_back(context_._accepts.size());
    context_._accepts.push_back(index_direct);

    terminals_direct_names.pop_back();
    terminals_direct_definitions.pop_back();
  }

  sort_terminals_by_name(context_);
}

void ParserBuilder::step_05(Context& context_) {
  if (context_._whitespace_definition.empty()) {
    return;
  }

  StateMachine fsm_whitespace;
  StateMachinePart fsm_part_whitespace = DefinitionToStateMachinePart::convert(fsm_whitespace, "@whitespace", context_._whitespace_definition, context_._terminal_names, context_._terminal_definitions, *context_._ast);
  StateNrType const state_nr_end = fsm_whitespace.create_new_state();
  fsm_part_whitespace.connect_outgoing_transitions_to(state_nr_end, fsm_whitespace);
  StateMachineDeterminizer::determinize(fsm_whitespace);

  std::stack<std::pair<StateNrType, size_t>> stack;
  IntervalSet<StateNrType> visited;

  auto const push_and_visit = [&stack, &visited](StateNrType state_nr_, size_t depth_) {
    if (visited.contains(state_nr_)) {
      return;
    }

    stack.push({state_nr_, depth_});
    visited.insert(Interval<StateNrType>(state_nr_));
  };

  push_and_visit(StateNrStart, 0);

  while (!stack.empty()) {
    auto const [state_nr_cur, depth_cur] = stack.top();
    stack.pop();

    State const& state_cur = *fsm_whitespace.get_state(state_nr_cur);

    IntervalSet<SymbolType> const symbols = state_cur.get_symbols();

    if (depth_cur > 0 && !symbols.empty()) {
      throw std::runtime_error("Whitespace definition too deep");
    }

    for (size_t i = 0; i < symbols.size(); ++i) {
      Interval<SymbolType> const symbol_interval = symbols.get_by_index(i);
      for (SymbolType symbol = symbol_interval.get_lower(); symbol <= symbol_interval.get_upper(); ++symbol) {
        context_._whitespace.insert(symbol);
        StateNrType const state_nr_next = state_cur.get_symbol_transition(Symbol(symbol));
        push_and_visit(state_nr_next, depth_cur + 1);
      }
    }
  }
}

void ParserBuilder::step_06(Context& context_) {
  for (size_t i = 0; i < context_._comment_open_definitions.size(); ++i) {
    StateMachine state_machine_comment_open;
    StateMachinePart state_machine_part_rule_open = DefinitionToStateMachinePart::convert(state_machine_comment_open, "@comment_open_" + std::to_string(i), context_._comment_open_definitions[i], context_._terminal_names, context_._terminal_definitions, *context_._ast);
    StateNrType const state_nr_end = state_machine_comment_open.create_new_state();
    state_machine_comment_open.get_state(state_nr_end)->get_accepts().resize(1, true);
    state_machine_part_rule_open.connect_outgoing_transitions_to(state_nr_end, state_machine_comment_open);
    StateMachineDeterminizer::determinize(state_machine_comment_open);
    context_._comment_open_fsms.push_back(std::move(state_machine_comment_open));
  }

  for (size_t i = 0; i < context_._comment_close_definitions.size(); ++i) {
    StateMachine state_machine_comment_close;
    StateMachinePart state_machine_part_rule_close = DefinitionToStateMachinePart::convert(state_machine_comment_close, "@comment_close_" + std::to_string(i), context_._comment_close_definitions[i], context_._terminal_names, context_._terminal_definitions, *context_._ast);
    StateNrType const state_nr_end = state_machine_comment_close.create_new_state();
    state_machine_comment_close.get_state(state_nr_end)->get_accepts().resize(1, true);
    state_machine_part_rule_close.connect_outgoing_transitions_to(state_nr_end, state_machine_comment_close);
    StateMachineDeterminizer::determinize(state_machine_comment_close);
    context_._comment_close_fsms.push_back(std::move(state_machine_comment_close));
  }

  for (size_t i = 0; i < context_._terminal_accepts.size(); ++i) {
    if (!context_._terminal_accepts[i].has_value() || context_._terminal_names[i] == "@eoi" || context_._terminal_names[i] == "@start") {
      continue;
    }

    StateMachine state_machine_terminal;
    StateMachinePart state_machine_part_terminal = DefinitionToStateMachinePart::convert(state_machine_terminal, context_._terminal_id_names[i], context_._terminal_definitions[i], context_._terminal_names, context_._terminal_definitions, *context_._ast);
    StateNrType const state_nr_end = state_machine_terminal.create_new_state();
    State& state_end = *state_machine_terminal.get_state(state_nr_end);
    state_end.get_accepts().resize(context_._accepts.size(), false);
    state_end.get_accepts().set(*context_._terminal_accepts[i], true);
    state_machine_part_terminal.connect_outgoing_transitions_to(state_nr_end, state_machine_terminal);
    StateMachineDeterminizer::determinize(state_machine_terminal);
    context_._terminal_fsms.push_back(std::move(state_machine_terminal));
  }
}

void ParserBuilder::step_07(Context& context_) {
  for (size_t i = 0; i < context_._comment_close_definitions.size(); ++i) {
    StateMachine& state_machine_comment_close = context_._comment_close_fsms[i];

    std::vector<StateNrType> pending;
    std::unordered_set<StateNrType> visited;

    auto const push_and_visit = [&pending, &visited](StateNrType state_nr_) {
      if (visited.contains(state_nr_)) {
        return;
      }

      pending.push_back(state_nr_);
      visited.insert(state_nr_);
    };

    push_and_visit(StateNrStart);

    while (!pending.empty()) {
      StateNrType const state_nr_cur = pending.back();
      pending.pop_back();

      State& state_cur = *state_machine_comment_close.get_state(state_nr_cur);

      if (state_cur.get_accepts().popcnt() == 0) {
        for (SymbolType symbol = 0; symbol < UCHAR_MAX; ++symbol) {
          StateNrType const state_nr_next = state_cur.get_symbol_transition(Symbol(symbol));
          if (state_nr_next != StateNrSink) {
            push_and_visit(state_nr_next);
          } else {
            state_cur.add_symbol_transition(Symbol(symbol), StateNrStart);
          }
        }
      }
    }
  }
}

void ParserBuilder::step_08(Context& context_) {
  context_._fsm.get_or_create_state(StateNrStart);

  for (size_t i = 0; i < context_._comment_open_fsms.size(); ++i) {
    StateMachine& state_machine_comment_open = context_._comment_open_fsms[i];
    StateNrType const state_nr_comment_open = context_._fsm.create_new_state();
    StateMachinePruner::prune(state_machine_comment_open, StateNrStart, state_nr_comment_open, true);

    StateMachine& state_machine_comment_close = context_._comment_close_fsms[i];
    StateNrType const state_nr_comment_close = state_nr_comment_open + state_machine_comment_close.get_state_count();
    StateMachinePruner::prune(state_machine_comment_close, StateNrStart, state_nr_comment_close, true);

    context_._fsm.get_state(StateNrStart)->add_epsilon_transition(state_nr_comment_open);

    {
      IntervalSet<StateNrType> const state_nrs = state_machine_comment_open.get_state_nrs();

      for (size_t j = 0; j < state_nrs.size(); ++j) {
        Interval<StateNrType> const interval = state_nrs.get_by_index(j);
        for (StateNrType state_nr = interval.get_lower(); state_nr <= interval.get_upper(); ++state_nr) {
          State& state = *state_machine_comment_open.get_state(state_nr);

          if (state.get_accepts().popcnt() != 0) {
            state.get_accepts().clear();
            state.add_epsilon_transition(state_nr_comment_close);
          }

          context_._fsm.get_or_create_state(state_nr) = state;
        }
      }
    }

    {
      IntervalSet<StateNrType> const state_nrs = state_machine_comment_close.get_state_nrs();

      for (size_t j = 0; j < state_nrs.size(); ++j) {
        Interval<StateNrType> const interval = state_nrs.get_by_index(j);
        for (StateNrType state_nr = interval.get_lower(); state_nr <= interval.get_upper(); ++state_nr) {
          State& state = *state_machine_comment_close.get_state(state_nr);

          if (state.get_accepts().popcnt() != 0) {
            state.get_accepts().clear();
            state.add_epsilon_transition(StateNrStart);
          }

          context_._fsm.get_or_create_state(state_nr) = state;
        }
      }
    }

    State& state_comment_open = *context_._fsm.get_state(state_nr_comment_open);

    for (SymbolType const symbol : context_._whitespace) {
      if (state_comment_open.get_symbol_transition(Symbol(symbol)) != StateNrSink) {
        throw std::runtime_error("Comment open conflicts with whitespace");
      }

      state_comment_open.add_symbol_transition(Symbol(symbol), StateNrStart);
    }
  }
}

void ParserBuilder::step_09(Context& context_) {
  State& state_start = context_._fsm.get_or_create_state(StateNrStart);
  state_start.get_accepts().resize(context_._accepts.size(), false);
  std::optional<size_t> const index_start = base::binary_find_index(context_._terminal_names.begin(), context_._terminal_names.end(), "@start");
  state_start.get_accepts().set(*context_._terminal_accepts[*index_start], true);

  StateNrType const state_nr_eoi = context_._fsm.create_new_state();
  State& state_eoi = *context_._fsm.get_state(state_nr_eoi);
  state_eoi.get_accepts().resize(context_._accepts.size(), false);
  std::optional<size_t> const index_eoi = base::binary_find_index(context_._terminal_names.begin(), context_._terminal_names.end(), "@eoi");
  state_eoi.get_accepts().set(*context_._terminal_accepts[*index_eoi], true);

  state_start.add_symbol_transition(Symbol(SymbolEoi), state_nr_eoi);
}

void ParserBuilder::step_10(Context& context_) {
  State& state_start = *context_._fsm.get_state(StateNrStart);

  for (size_t i = 0; i < context_._terminal_fsms.size(); ++i) {
    StateMachine& state_machine_terminal = context_._terminal_fsms[i];
    StateNrType const state_nr_terminal = context_._fsm.get_unused_state_nr();
    StateMachinePruner::prune(state_machine_terminal, StateNrStart, state_nr_terminal, true);

    {
      IntervalSet<StateNrType> const state_nrs = state_machine_terminal.get_state_nrs();
      for (size_t j = 0; j < state_nrs.size(); ++j) {
        Interval<StateNrType> const interval = state_nrs.get_by_index(j);
        for (StateNrType state_nr = interval.get_lower(); state_nr <= interval.get_upper(); ++state_nr) {
          context_._fsm.get_or_create_state(state_nr) = *state_machine_terminal.get_state(state_nr);
        }
      }
    }

    state_start.add_epsilon_transition(state_nr_terminal);

    State& state_terminal = *context_._fsm.get_state(state_nr_terminal);

    for (SymbolType const symbol : context_._whitespace) {
      if (state_terminal.get_symbol_transition(Symbol(symbol)) != StateNrSink) {
        throw std::runtime_error("Terminal conflicts with whitespace");
      }

      state_terminal.add_symbol_transition(Symbol(symbol), StateNrStart);
    }
  }
}

void ParserBuilder::step_11(Context& context_) {
  StateMachineDeterminizer::determinize(context_._fsm);
  StateMachineMinimizer::minimize(context_._fsm);

  Bitset accepts_start = context_._fsm.get_state(StateNrStart)->get_accepts();

  size_t const index_start = *context_._terminal_accepts[*pmt::base::binary_find_index(context_._terminal_names.begin(), context_._terminal_names.end(), "@start")];
  accepts_start.set(index_start, false);

  if (accepts_start.any()) {
    static size_t const MAX_REPORTED = 8;
    std::unordered_set<size_t> const accepts_start_set = pmt::base::BitsetConverter::to_unordered_set(accepts_start);
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
  IntervalSet<StateNrType> const state_nrs = context_._fsm.get_state_nrs();
  for (size_t j = 0; j < state_nrs.size(); ++j) {
    Interval<StateNrType> const interval = state_nrs.get_by_index(j);
    for (StateNrType state_nr = interval.get_lower(); state_nr <= interval.get_upper(); ++state_nr) {
      State const& state = *context_._fsm.get_state(state_nr);
      if (state.get_accepts().size() == 0) {
        continue;
      }
      accepts_start_count += state.get_accepts().get(index_start) == true ? 1 : 0;
    }
  }

  if (accepts_start_count != 1) {
    throw std::runtime_error("Error during building, there are " + std::to_string(accepts_start_count) + " starting states");
  }
}

void ParserBuilder::write_dot(Context& context_, pmt::util::smct::StateMachine const& state_machine_) {
  if (state_machine_.get_state_count() > DOT_FILE_MAX_STATES) {
    std::cerr << "Skipping dot file write, too many states\n";
    return;
  }

  std::ofstream file(DOT_FILE_PREFIX + std::to_string(context_._dot_file_count++) + ".dot");
  GraphWriter::write_dot(file, state_machine_, [&context_](size_t accepts_) { return accepts_to_label(context_, accepts_); });
}

auto ParserBuilder::accepts_to_label(Context& context_, size_t accept_idx_) -> std::string {
  return context_._terminal_names[context_._accepts[accept_idx_]];
}

void ParserBuilder::sort_terminals_by_name(Context& context_) {
  std::vector<size_t> ordering;
  std::generate_n(std::back_inserter(ordering), context_._terminal_names.size(), [i = 0]() mutable { return i++; });
  std::ranges::sort(ordering, [&context_](size_t lhs_, size_t rhs_) { return context_._terminal_names[lhs_] < context_._terminal_names[rhs_]; });

  apply_permutation(context_._terminal_names.begin(), context_._terminal_names.end(), ordering.begin());
  apply_permutation(context_._terminal_id_names.begin(), context_._terminal_id_names.end(), ordering.begin());
  apply_permutation(context_._terminal_accepts.begin(), context_._terminal_accepts.end(), ordering.begin());
  apply_permutation(context_._terminal_case_sensitive_present, ordering.begin());
  apply_permutation(context_._terminal_case_sensitive_values, ordering.begin());
  apply_permutation(context_._terminal_definitions.begin(), context_._terminal_definitions.end(), ordering.begin());

  std::vector<size_t> const ordering_inverse = inverse_permutation(ordering.begin(), ordering.end());

  std::transform(context_._accepts.begin(), context_._accepts.end(), context_._accepts.begin(), [&ordering_inverse](size_t i_) { return ordering_inverse[i_]; });
}

void ParserBuilder::sort_rules_by_name(Context& context_) {
  std::vector<size_t> ordering;
  std::generate_n(std::back_inserter(ordering), context_._rule_names.size(), [i = 0]() mutable { return i++; });
  std::ranges::sort(ordering, [&context_](size_t lhs_, size_t rhs_) { return context_._rule_names[lhs_] < context_._rule_names[rhs_]; });

  apply_permutation(context_._rule_names.begin(), context_._rule_names.end(), ordering.begin());
  apply_permutation(context_._rule_id_names.begin(), context_._rule_id_names.end(), ordering.begin());
  apply_permutation(context_._rule_merge_values, ordering.begin());
  apply_permutation(context_._rule_unpack_values, ordering.begin());
  apply_permutation(context_._rule_hide_values, ordering.begin());
  apply_permutation(context_._rule_definitions.begin(), context_._rule_definitions.end(), ordering.begin());
}

auto ParserBuilder::Context::try_find_terminal_definition(std::string const& name_) -> pmt::util::smrt::GenericAstPath {
  std::optional<size_t> const index = base::binary_find_index(_terminal_names.begin(), _terminal_names.end(), name_);
  if (!index.has_value()) {
    throw std::runtime_error("Terminal not found: " + name_);
  }

  return _terminal_definitions[*index];
}

auto ParserBuilder::Context::try_find_rule_definition(std::string const& name_) -> pmt::util::smrt::GenericAstPath {
  std::optional<size_t> const index = base::binary_find_index(_rule_names.begin(), _rule_names.end(), name_);
  if (!index.has_value()) {
    throw std::runtime_error("Rule not found: " + name_);
  }

  return _rule_definitions[*index];
}

}  // namespace pmt::parserbuilder
