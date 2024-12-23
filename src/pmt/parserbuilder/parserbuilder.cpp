#include "pmt/parserbuilder/parserbuilder.hpp"

#include "pmt/asserts.hpp"
#include "pmt/base/algo.hpp"
#include "pmt/base/dynamic_bitset.hpp"
#include "pmt/base/dynamic_bitset_converter.hpp"
#include "pmt/parserbuilder/grm_ast.hpp"
#include "pmt/parserbuilder/grm_lexer.hpp"
#include "pmt/parserbuilder/grm_parser.hpp"
#include "pmt/parserbuilder/lexer_builder.hpp"
#include "pmt/parserbuilder/table_writer.hpp"
#include "pmt/parserbuilder/terminal_definition_to_fa_part.hpp"
#include "pmt/util/parsect/graph_writer.hpp"
#include "pmt/util/parsert/generic_ast.hpp"
#include "pmt/util/parsert/generic_ast_printer.hpp"
#include "pmt/util/parsert/generic_lexer.hpp"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <stack>

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

  step_1(context);
  step_2(context);
  step_3(context);
  step_4(context);
  step_5(context);

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
}

void ParserBuilder::step_1(Context& context_) {
  for (size_t i = 0; i < context_._ast->get_children_size(); ++i) {
    GenericAst const& child = *context_._ast->get_child_at(i);
    switch (child.get_id()) {
      case GrmAst::NtGrammarProperty:
        step_1_handle_grammar_property(context_, GenericAst::AstPositionConst{context_._ast.get(), i});
        break;
      case GrmAst::NtTerminalProduction:
        step_1_handle_terminal_production(context_, GenericAst::AstPositionConst{context_._ast.get(), i});
        break;
      case GrmAst::NtRuleProduction:
        step_1_handle_rule_production(context_, GenericAst::AstPositionConst{context_._ast.get(), i});
        break;
    }
  }
}

void ParserBuilder::step_1_handle_grammar_property(Context& context_, GenericAst::AstPositionConst position_) {
  GenericAst const& grammar_property = *position_.first->get_child_at(position_.second);
  GenericAst const& property_name = *grammar_property.get_child_at(0);
  GenericAst::AstPositionConst const property_value_position(&grammar_property, 1);
  switch (property_name.get_id()) {
    case GrmAst::TkGrammarPropertyCaseSensitive:
      step_1_handle_grammar_property_case_sensitive(context_, property_value_position);
      break;
    case GrmAst::TkGrammarPropertyStart:
      step_1_handle_grammar_property_start(context_, property_value_position);
      break;
    case GrmAst::TkGrammarPropertyWhitespace:
      step_1_handle_grammar_property_whitespace(context_, property_value_position);
      break;
    case GrmAst::TkGrammarPropertyComment:
      step_1_handle_grammar_property_comment(context_, property_value_position);
      break;
    default:
      pmt_unreachable();
  }
}

void ParserBuilder::step_1_handle_grammar_property_case_sensitive(Context& context_, GenericAst::AstPositionConst position_) {
  GenericAst const& value = *position_.first->get_child_at(position_.second);
  assert(value.get_id() == GrmAst::TkBooleanLiteral);
  context_._case_sensitive = value.get_string() == "true";
}

void ParserBuilder::step_1_handle_grammar_property_start(Context& context_, GenericAst::AstPositionConst position_) {
  GenericAst const& value = *position_.first->get_child_at(position_.second);
  assert(value.get_id() == GrmAst::TkRuleIdentifier);
  context_._start_symbol = value.get_string();
}

void ParserBuilder::step_1_handle_grammar_property_whitespace(Context& context_, GenericAst::AstPositionConst position_) {
  context_._whitespace_definition = position_;
}

void ParserBuilder::step_1_handle_grammar_property_comment(Context& context_, GenericAst::AstPositionConst position_) {
  GenericAst const& terminal_definition_pair_list = *position_.first->get_child_at(position_.second);
  for (size_t i = 0; i < terminal_definition_pair_list.get_children_size(); ++i) {
    GenericAst const& terminal_definition_pair = *terminal_definition_pair_list.get_child_at(i);
    context_._comment_open_definitions.push_back(GenericAst::AstPositionConst{&terminal_definition_pair, 0});
    context_._comment_close_definitions.push_back(GenericAst::AstPositionConst{&terminal_definition_pair, 1});
  }
}

void ParserBuilder::step_1_handle_terminal_production(Context& context_, GenericAst::AstPositionConst position_) {
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
  context_._terminal_case_sensitive_values.push_back(terminal_case_sensitive.value_or(false));
  context_._terminal_definitions.push_back(terminal_definition_position);
}

void ParserBuilder::step_1_handle_rule_production(Context& context_, GenericAst::AstPositionConst position_) {
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

void ParserBuilder::step_2(Context& context_) {
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

void ParserBuilder::step_3(Context& context_) {
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

void ParserBuilder::step_4(Context& context_) {
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

void ParserBuilder::step_5(Context& context_) {
  context_._terminal_accepts.resize(context_._terminal_names.size(), std::nullopt);

  size_t accept_count = 0;
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
        context_._terminal_accepts[*index] = accept_count++;
        push_and_visit(context_._terminal_definitions[*index]);
      } break;
      case GrmAst::TkRuleIdentifier:
        push_and_visit(context_.try_find_rule_definition(cur.get_string()));
        break;
      case GrmAst::NtTerminalDefinition:
      case GrmAst::NtRuleDefinition:
      case GrmAst::NtTerminalChoices:
      case GrmAst::NtTerminalSequence:
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

void ParserBuilder::step_6(Context& context_) {
 Fa::StateNrType const state_nr_start = context_._fa.get_unused_state_nr();
 context_._fa._states[state_nr_start];

 for (size_t i = 0; i < context_._comment_open_definitions.size(); ++i) {
  
 }

}

void ParserBuilder::write_dot(Context& context_, pmt::util::parsect::Fa const& fa_) {
  if (fa_._states.size() > DOT_FILE_MAX_STATES) {
    std::cerr << "Skipping dot file write, too many states\n";
    return;
  }

  std::ofstream file(DOT_FILE_PREFIX + std::to_string(context_._dot_file_count++) + ".dot");
  GraphWriter::write_dot(file, fa_, [](size_t accepts_) { return "..."; });
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