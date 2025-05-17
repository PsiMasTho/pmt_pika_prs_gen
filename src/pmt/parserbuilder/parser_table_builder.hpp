#pragma once

#include "pmt/fw_decl.hpp"
#include "pmt/parserbuilder/parser_tables.hpp"
#include "pmt/parserbuilder/nonterminal_state_machine_part_builder.hpp"
#include "pmt/util/smrt/generic_ast_path.hpp"

PMT_FW_DECL_NS_CLASS(pmt::util::smrt, GenericAst);
PMT_FW_DECL_NS_CLASS(pmt::parserbuilder, GrammarData);
PMT_FW_DECL_NS_CLASS(pmt::parserbuilder, LexerTables);

namespace pmt::parserbuilder {

class ParserTableBuilder {
 // -$ Data $-
 NonterminalStateMachinePartBuilder _nonterminal_state_machine_part_builder;
 ParserTables _result_tables;
 pmt::util::smrt::GenericAst const* _ast = nullptr;
 GrammarData const* _grammar_data = nullptr;
 LexerTables const* _lexer_tables = nullptr;
 pmt::base::IntervalSet<pmt::util::smrt::StateNrType> _conflicting_state_nrs;

 public:
  // -$ Functions $-
  // --$ Other $--
  auto build(pmt::util::smrt::GenericAst const& ast_, GrammarData const& grammar_data_, LexerTables const& lexer_tables_) -> ParserTables;

 private:
 void setup_parser_state_machine();
 void extract_conflicts();
 void setup_lookahead_state_machine();

 void write_dot(std::string_view filename_, std::string title_, pmt::util::smct::StateMachine const& state_machine_, bool is_lookahead_ = false) const;

 auto lookup_nonterminal_label_by_index(size_t index_) const -> std::string;
 auto lookup_nonterminal_index_by_label(std::string_view label_) const -> size_t;
 auto lookup_nonterminal_definition_by_index(size_t index_) const -> pmt::util::smrt::GenericAstPath;

 void find_conflicting_state_nrs();
 static void try_solve_partial_conflict(pmt::util::smct::StateMachine& state_machine_, pmt::base::IntervalSet<pmt::util::smrt::SymbolType> conflict_symbols_);
};

}