#pragma once

#include "pmt/fw_decl.hpp"
#include "pmt/parserbuilder/lexer_tables.hpp"
#include "pmt/util/smrt/generic_ast_path.hpp"

#include <string_view>
#include <vector>

PMT_FW_DECL_NS_CLASS(pmt::util::smrt, GenericAst);
PMT_FW_DECL_NS_CLASS(pmt::parserbuilder, GrammarData);


namespace pmt::parserbuilder {

class LexerTableBuilder {
  // -$ Data $-
  LexerTables _result_tables;
  std::vector<pmt::util::smct::StateMachine> _terminal_state_machines;
  std::vector<pmt::util::smct::StateMachine> _comment_open_state_machines;
  std::vector<pmt::util::smct::StateMachine> _comment_close_state_machines;
  pmt::util::smct::StateMachine _whitespace_state_machine;
  pmt::util::smrt::GenericAst const* _ast = nullptr;
  GrammarData const* _grammar_data = nullptr;

 public:
  // -$ Functions $-
  // --$ Other $--
  auto build(pmt::util::smrt::GenericAst const& ast_, GrammarData const& grammar_data_) -> LexerTables;

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

  void write_dot(std::string_view filename_, pmt::util::smct::StateMachine const& state_machine_) const;
};

}  // namespace pmt::parserbuilder
