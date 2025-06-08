#pragma once

#include "pmt/fw_decl.hpp"
#include "pmt/parser/builder/lexer_tables.hpp"
#include "pmt/parser/generic_ast_path.hpp"

#include <string_view>
#include <vector>

PMT_FW_DECL_NS_CLASS(pmt::parser, GenericAst);
PMT_FW_DECL_NS_CLASS(pmt::parser::grammar, GrammarData);


namespace pmt::parser::builder {

class LexerTableBuilder {
  // -$ Data $-
  LexerTables _result_tables;
  pmt::util::sm::ct::StateMachine _lexer_state_machine;
  pmt::util::sm::ct::StateMachine _linecount_state_machine;
  std::vector<pmt::util::sm::ct::StateMachine> _terminal_state_machines;
  std::vector<pmt::util::sm::ct::StateMachine> _comment_open_state_machines;
  std::vector<pmt::util::sm::ct::StateMachine> _comment_close_state_machines;
  pmt::util::sm::ct::StateMachine _whitespace_state_machine;
  GenericAst const* _ast = nullptr;
  pmt::parser::grammar::GrammarData const* _grammar_data = nullptr;

 public:
  // -$ Functions $-
  // --$ Other $--
  auto build(GenericAst const& ast_, pmt::parser::grammar::GrammarData const& grammar_data_) -> LexerTables;

 private:
  void setup_whitespace_state_machine();
  void setup_terminal_state_machines();
  void setup_comment_state_machines();
  void setup_linecount_state_machine();
  void setup_default_linecount_state_machine();
  void loop_back_comment_close_state_machines();
  void merge_comment_state_machines_into_result();
  void merge_whitespace_state_machine_into_result();
  void setup_start_and_eoi_states();
  void connect_terminal_state_machines();
  void loop_back_linecount_state_machine();
  void fill_terminal_data();
  void validate_result();

  void write_dot(std::string_view filename_, pmt::util::sm::ct::StateMachine const& state_machine_) const;
};

}  // namespace pmt::parserbuilder
