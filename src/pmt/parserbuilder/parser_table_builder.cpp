#include "pmt/parserbuilder/parser_table_builder.hpp"

#include "pmt/parserbuilder/grammar_data.hpp"
#include "pmt/parserbuilder/lexer_tables.hpp"
#include "pmt/util/smct/state_machine_part.hpp"
#include "pmt/util/smct/state_machine_determinizer.hpp"
#include "pmt/util/smct/state_machine_minimizer.hpp"
#include "pmt/util/smct/graph_writer.hpp"
#include "pmt/util/smct/state.hpp"

#include <iostream>
#include <fstream>

namespace pmt::parserbuilder {
 using namespace pmt::base;
 using namespace pmt::util::smct;
 using namespace pmt::util::smrt;

auto ParserTableBuilder::build(GenericAst const& ast_, GrammarData const& grammar_data_, LexerTables const& lexer_tables_) -> ParserTables {
 _ast = &ast_;
 _grammar_data = &grammar_data_;
 _lexer_tables = &lexer_tables_;

 setup_parser_state_machine();
 write_dot("parser_state_machine.dot", _result_tables._parser_state_machine);

 return std::move(_result_tables);
}

void ParserTableBuilder::setup_parser_state_machine() {
 StateMachinePart state_machine_part_nonterminal = _nonterminal_state_machine_part_builder.build([this](size_t index_) { return lookup_nonterminal_label_by_index(index_); }, [this](std::string_view name_) { return lookup_nonterminal_index_by_label(name_); }, [this](size_t index_) { return lookup_nonterminal_definition_by_index(index_); }, [&](std::string_view label_){return *_lexer_tables->terminal_label_to_index(label_);}, *_ast, _grammar_data->_start_nonterminal_definition, _result_tables._parser_state_machine);

 StateNrType const state_nr_pre_end = _result_tables._parser_state_machine.create_new_state();
 State& state_pre_end = *_result_tables._parser_state_machine.get_state(state_nr_pre_end);
 StateNrType const state_nr_end = _result_tables._parser_state_machine.create_new_state();
 State& state_end = *_result_tables._parser_state_machine.get_state(state_nr_end);
 state_pre_end.add_symbol_transition(Symbol(SymbolKindTerminal, _lexer_tables->get_eoi_terminal_index()), state_nr_end);
 state_end.get_accepts().resize(_grammar_data->_nonterminals.size(), false);
 size_t const index_eoi_nonterminal = binary_find_index(_grammar_data->_nonterminals.begin(), _grammar_data->_nonterminals.end(), GrammarData::LABEL_EOI, [](auto const& lhs_, auto const& rhs_) { return GrammarData::FetchLabelString{}(lhs_) < GrammarData::FetchLabelString{}(rhs_); });
 state_end.get_accepts().set(index_eoi_nonterminal, true);

 state_machine_part_nonterminal.connect_outgoing_transitions_to(state_nr_pre_end, _result_tables._parser_state_machine);

 // The starting state may not be StateNrStart, so determinize from the incoming state nr
 StateMachineDeterminizer::determinize(_result_tables._parser_state_machine, *state_machine_part_nonterminal.get_incoming_state_nr());
 StateMachineMinimizer::minimize(_result_tables._parser_state_machine);
}

void ParserTableBuilder::write_dot(std::string_view filename_, pmt::util::smct::StateMachine const& state_machine_) const {
 static size_t const DOT_FILE_MAX_STATES = 750;
 if (state_machine_.get_state_count() > DOT_FILE_MAX_STATES) {
  std::cerr << "Skipping dot file write of " << filename_ << " because it has " << state_machine_.get_state_count() << " states, which is more than the limit of " << DOT_FILE_MAX_STATES << '\n';
  return;
 }

 std::ofstream graph_file(filename_.data());
 std::ifstream skel_file("/home/pmt/repos/pmt/skel/pmt/util/smct/state_machine-skel.dot");

 GraphWriter::WriterArgs writer_args{
  ._os_graph = graph_file,
  ._is_graph_skel = skel_file,
  ._state_machine = state_machine_
 };

 GraphWriter::StyleArgs style_args;
 style_args._accepts_to_label_fn = [&](size_t accepts_) -> std::string {
  return lookup_nonterminal_label_by_index(accepts_);
 };
 style_args._symbols_to_label_fn = [&](IntervalSet<SymbolType> const& symbols_) -> std::string {
  std::string delim;
  std::string ret;
  symbols_.for_each_key([&](SymbolType symbol_) {
   Symbol symbol(symbol_);
   switch (symbol.get_kind()) {
    case SymbolKindTerminal:
     ret += std::exchange(delim, ", ") + _lexer_tables->get_terminal_label(symbol.get_value());
     break;
    case SymbolKindOpen:
     ret += std::exchange(delim, ", ") + "+";
     break;
    case SymbolKindClose:
     ret = lookup_nonterminal_label_by_index(symbol.get_value());
     return;
    default:
     ret += std::exchange(delim, ", ") + "unknown";
     break;
   }
  });
  return ret;
 };
 style_args._symbol_kind_to_edge_style_fn = [&](SymbolType kind_) -> GraphWriter::EdgeStyle {
  switch (kind_) {
   case SymbolKindTerminal:
    return GraphWriter::EdgeStyle::Solid;
   case SymbolKindOpen:
    return GraphWriter::EdgeStyle::Solid;
   case SymbolKindClose:
    return GraphWriter::EdgeStyle::Dashed;
   default:
    return GraphWriter::EdgeStyle::Solid;
  }
 };

 style_args._symbol_kind_to_font_flags_fn = [&](SymbolType kind_) -> GraphWriter::FontFlags {
  switch (kind_) {
   case SymbolKindTerminal:
    return GraphWriter::FontFlags::Italic;
   case SymbolKindClose:
    return GraphWriter::FontFlags::Bold;
   default:
    return GraphWriter::FontFlags::None;
  }
 };

 style_args._layout_direction = GraphWriter::LayoutDirection::TopToBottom;

 GraphWriter().write_dot(writer_args, style_args);
 std::cout << "Wrote dot file: " << filename_ << '\n';
}

auto ParserTableBuilder::lookup_nonterminal_label_by_index(size_t index_) const -> std::string {
 assert(index_ < _grammar_data->_nonterminals.size());
 return _grammar_data->_nonterminals[index_]._label;
}

auto ParserTableBuilder::lookup_nonterminal_index_by_label(std::string_view label_) const -> size_t {
 size_t const index = binary_find_index(_grammar_data->_nonterminals.begin(), _grammar_data->_nonterminals.end(), label_, [](auto const& lhs_, auto const& rhs_) { return GrammarData::FetchLabelString{}(lhs_) < GrammarData::FetchLabelString{}(rhs_); });
 assert(index != _grammar_data->_nonterminals.size());
 return index;
}

auto ParserTableBuilder::lookup_nonterminal_definition_by_index(size_t index_) const -> GenericAstPath {
 assert(index_ < _grammar_data->_nonterminals.size());
 return _grammar_data->_nonterminals[index_]._definition_path;
}

}