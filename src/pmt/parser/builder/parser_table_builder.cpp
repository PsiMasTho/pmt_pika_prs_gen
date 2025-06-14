#include "pmt/parser/builder/parser_table_builder.hpp"

#include "pmt/parser/builder/lexer_tables.hpp"
#include "pmt/parser/builder/parser_lookahead_builder.hpp"
#include "pmt/parser/builder/parser_reachability_checker.hpp"
#include "pmt/parser/builder/state_machine_part_builder.hpp"
#include "pmt/parser/grammar/ast.hpp"
#include "pmt/parser/grammar/grammar_data.hpp"
#include "pmt/parser/primitives.hpp"
#include "pmt/util/sm/ct/graph_writer.hpp"
#include "pmt/util/sm/ct/state.hpp"
#include "pmt/util/sm/ct/state_machine_determinizer.hpp"
#include "pmt/util/sm/ct/state_machine_minimizer.hpp"
#include "pmt/util/sm/ct/state_machine_part.hpp"

#include <fstream>
#include <iostream>

namespace pmt::parser::builder {
using namespace pmt::base;
using namespace pmt::util::sm::ct;
using namespace pmt::util::sm;
using namespace pmt::parser::grammar;

namespace {
size_t const DOT_FILE_MAX_STATES = 500;

class Locals {
 public:
  ParserTables _result_tables;
  pmt::util::sm::ct::StateMachine _parser_state_machine;
  size_t _nonterminal_dotfile_counter = 0;
};

void setup_parser_state_machine(ParserTableBuilder::Args const& args_, Locals& locals_) {
  StateNrType const state_nr_start = locals_._parser_state_machine.create_new_state();
  StateMachinePart state_machine_part_nonterminal = StateMachinePartBuilder::build(StateMachinePartBuilder::NonterminalBuildingArgs(
   StateMachinePartBuilder::ArgsBase{._ast_root = args_._ast, ._dest_state_machine = locals_._parser_state_machine, ._fn_lookup_definition = [&](size_t index_) { return args_._grammar_data.lookup_nonterminal_definition_by_index(index_); }, ._starting_index = args_._grammar_data._nonterminal_accepts.size()}, [&](size_t index_) { return args_._grammar_data.lookup_nonterminal_name_by_index(index_); }, [&](std::string_view name_) { return args_._grammar_data.lookup_nonterminal_index_by_name(name_); },
   [&](std::string_view label_) { return *args_._lexer_tables.terminal_label_to_index(label_); }));
  StateNrType const state_nr_pre_end = locals_._parser_state_machine.create_new_state();
  State& state_pre_end = *locals_._parser_state_machine.get_state(state_nr_pre_end);
  StateNrType const state_nr_end = locals_._parser_state_machine.create_new_state();
  State& state_end = *locals_._parser_state_machine.get_state(state_nr_end);
  state_pre_end.add_symbol_transition(Symbol(SymbolKindHiddenTerminal, args_._lexer_tables.get_eoi_accept_index()), state_nr_end);
  state_end.get_accepts().insert(Interval<AcceptsIndexType>(binary_find_index(args_._grammar_data._nonterminal_accepts.begin(), args_._grammar_data._nonterminal_accepts.end(), Ast::NAME_EOI, [](auto const& lhs_, auto const& rhs_) { return GrammarData::FetchLabelString{}(lhs_) < GrammarData::FetchLabelString{}(rhs_); })));
  state_machine_part_nonterminal.connect_outgoing_transitions_to(state_nr_pre_end, locals_._parser_state_machine);
  locals_._parser_state_machine.get_state(state_nr_start)->add_epsilon_transition(*state_machine_part_nonterminal.get_incoming_state_nr());
}

void fill_nonterminal_data(ParserTableBuilder::Args const& args_, Locals& locals_) {
  for (GrammarData::NonterminalAcceptData const& accept_data : args_._grammar_data._nonterminal_accepts) {
    locals_._result_tables.add_nonterminal_data(accept_data._name, args_._grammar_data.get_non_generic_ids().find(accept_data._id_string)->second, accept_data._merge, accept_data._unpack, accept_data._hide);
  }
}

void write_nonterminal_state_machine_dot(ParserTableBuilder::Args const& args_, Locals& locals_, std::string title_, pmt::util::sm::ct::StateMachine const& state_machine_) {
  std::string filename = "nonterminal_state_machine_" + std::to_string(locals_._nonterminal_dotfile_counter++) + ".dot";
  if (state_machine_.get_state_count() > DOT_FILE_MAX_STATES) {
    std::cerr << "Skipping dot file write of " << filename << " because it has " << state_machine_.get_state_count() << " states, which is more than the limit of " << DOT_FILE_MAX_STATES << '\n';
    return;
  }

  if (state_machine_.get_state_count() == 0) {
    std::cerr << "Skipping dot file write of " << filename << " because it has no states\n";
    return;
  }

  std::ofstream graph_file(filename);
  std::ifstream skel_file("/home/pmt/repos/pmt/skel/pmt/util/smct/state_machine-skel.dot");

  GraphWriter::WriterArgs writer_args{._os_graph = graph_file, ._is_graph_skel = skel_file, ._state_machine = state_machine_};

  GraphWriter::StyleArgs style_args;
  style_args._accepts_to_label_fn = [&](size_t accepts_) -> std::string {
    return args_._grammar_data.lookup_nonterminal_name_by_index(accepts_);
  };

  style_args._title = std::move(title_);
  style_args._symbols_to_label_fn = [&](SymbolKindType kind_, IntervalSet<SymbolValueType> const& symbols_) -> std::string {
    std::string delim;
    std::string ret;

    switch (kind_) {
      case SymbolKindTerminal:
      case SymbolKindHiddenTerminal:
        symbols_.for_each_key([&](SymbolValueType symbol_) { ret += std::exchange(delim, ", ") + args_._lexer_tables.get_accept_index_label(symbol_); });
        break;
      case SymbolKindOpen:
        symbols_.for_each_key([&](SymbolValueType symbol_) { ret += std::exchange(delim, ", ") + "+"; });
        break;
      case SymbolKindClose:
        symbols_.for_each_key([&](SymbolValueType symbol_) { ret += std::exchange(delim, ", ") + args_._grammar_data.lookup_nonterminal_name_by_index(symbol_); });
        break;
      case SymbolKindConflict:
        symbols_.for_each_key([&](SymbolValueType symbol_) { ret += std::exchange(delim, ", ") + std::to_string(symbol_); });
        break;
      default:
        symbols_.for_each_key([&](SymbolValueType symbol_) { ret += std::exchange(delim, ", ") + "unknown"; });
        break;
    }

    return ret;
  };
  style_args._symbol_kind_to_edge_style_fn = [&](SymbolValueType kind_) -> GraphWriter::EdgeStyle {
    switch (kind_) {
      case SymbolKindClose:
        return GraphWriter::EdgeStyle::Dashed;
      default:
        return GraphWriter::EdgeStyle::Solid;
    }
  };

  style_args._symbol_kind_to_font_flags_fn = [&](SymbolValueType kind_) -> GraphWriter::FontFlags {
    switch (kind_) {
      case SymbolKindTerminal:
      case SymbolKindHiddenTerminal:
        return GraphWriter::FontFlags::Italic;
      case SymbolKindClose:
        return GraphWriter::FontFlags::Bold;
      default:
        return GraphWriter::FontFlags::None;
    }
  };

  style_args._symbol_kind_to_edge_color_fn = [&](SymbolValueType kind_) -> GraphWriter::Color {
    switch (kind_) {
      case SymbolKindConflict:
        return GraphWriter::Color{._r = 255, ._g = 165, ._b = 0};
      case SymbolKindHiddenTerminal:
        return GraphWriter::Color{._r = 191, ._g = 191, ._b = 191};
      default:
        return GraphWriter::Color{._r = 0, ._g = 0, ._b = 0};
    }
  };

  style_args._symbol_kind_to_font_color_fn = [&](SymbolValueType kind_) -> GraphWriter::Color {
    switch (kind_) {
      case SymbolKindConflict:
        return GraphWriter::Color{._r = 255, ._g = 165, ._b = 0};
      case SymbolKindHiddenTerminal:
        return GraphWriter::Color{._r = 191, ._g = 191, ._b = 191};
      default:
        return GraphWriter::Color{._r = 0, ._g = 0, ._b = 0};
    }
  };

  style_args._layout_direction = GraphWriter::LayoutDirection::TopToBottom;

  GraphWriter().write_dot(writer_args, style_args);
  std::cout << "Wrote dot file: " << filename << '\n';
}

}  // namespace

auto ParserTableBuilder::build(Args args_) -> ParserTables {
  Locals locals;

  setup_parser_state_machine(args_, locals);
  write_nonterminal_state_machine_dot(args_, locals, "Initial tables", locals._parser_state_machine);
  ParserReachabilityChecker::check_reachability(ParserReachabilityChecker::Args{._fn_lookup_accept_index_by_label = [&](std::string_view label_) { return args_._grammar_data.lookup_nonterminal_index_by_name(label_); }, ._parser_state_machine = locals._parser_state_machine});
  StateMachineDeterminizer::determinize(StateMachineDeterminizer::Args{._state_machine = locals._parser_state_machine});
  write_nonterminal_state_machine_dot(args_, locals, "Determinized tables", locals._parser_state_machine);
  StateMachineMinimizer::minimize(locals._parser_state_machine);
  write_nonterminal_state_machine_dot(args_, locals, "Minimized tables", locals._parser_state_machine);
  IntervalSet<StateNrType> const conflicting_state_nrs = ParserLookaheadBuilder::extract_conflicts(locals._parser_state_machine);
  write_nonterminal_state_machine_dot(args_, locals, "Final tables", locals._parser_state_machine);
  StateMachine state_machine_lookahead = ParserLookaheadBuilder::build(ParserLookaheadBuilder::Args{._parser_state_machine = locals._parser_state_machine, ._conflicting_state_nrs = conflicting_state_nrs, ._fn_lookup_terminal_label = [&](size_t index_) {
                                                                                                      return args_._lexer_tables.get_accept_index_label(index_);
                                                                                                    }});

  fill_nonterminal_data(args_, locals);

  locals._result_tables.set_parser_state_machine(std::move(locals._parser_state_machine));
  locals._result_tables.set_lookahead_state_machine(std::move(state_machine_lookahead));

  return std::move(locals._result_tables);
}

}  // namespace pmt::parser::builder