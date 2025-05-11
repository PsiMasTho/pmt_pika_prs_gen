#pragma once

#include "pmt/fw_decl.hpp"
#include "pmt/parserbuilder/lexer_tables.hpp"
#include "pmt/parserbuilder/terminal_state_machine_part_builder.hpp"

#include <string_view>
#include <vector>

PMT_FW_DECL_NS_CLASS(pmt::util::smrt, GenericAst);
PMT_FW_DECL_NS_CLASS(pmt::parserbuilder, GrammarData);


namespace pmt::parserbuilder {

class LexerTableBuilder {
  // -$ Types / Constants $-
  enum : size_t {
    TerminalIndexAnonymous = std::numeric_limits<size_t>::max(),
  };

  // -$ Data $-
  TerminalStateMachinePartBuilder _terminal_state_machine_part_builder;
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
  void setup_newline_state_machine();
  void setup_default_newline_state_machine();
  void loop_back_comment_close_state_machines();
  void merge_comment_state_machines_into_result();
  void merge_whitespace_state_machine_into_result();
  void setup_start_and_eoi_states();
  void connect_terminal_state_machines();
  void loop_back_newline_state_machine();
  void fill_terminal_data();
  void validate_result();
  void validate_newline_state_machine();

  void write_dot(std::string_view filename_, pmt::util::smct::StateMachine const& state_machine_) const;

  auto lookup_terminal_label_by_index(size_t index_) const -> std::string;
  auto lookup_terminal_index_by_label(std::string_view label_) const -> size_t;
  auto lookup_terminal_definition_by_index(size_t index_) const -> pmt::util::smrt::GenericAstPath;
};

}  // namespace pmt::parserbuilder
