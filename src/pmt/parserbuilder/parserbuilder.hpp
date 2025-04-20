#pragma once

#include "pmt/base/bitset.hpp"
#include "pmt/util/smct/state_machine.hpp"
#include "pmt/util/smrt/generic_ast.hpp"
#include "pmt/util/smrt/generic_ast_path.hpp"

#include <optional>
#include <set>
#include <string>
#include <string_view>
#include <vector>

namespace pmt::parserbuilder {

class ParserBuilder {
  class Context;

 public:
  static void build(std::string_view input_grammar_path_);

 private:
  static void step_01(Context& context_);
  static void step_01_handle_grammar_property(Context& context_, pmt::util::smrt::GenericAstPath const& path_);
  static void step_01_handle_grammar_property_case_sensitive(Context& context_, pmt::util::smrt::GenericAstPath const& path_);
  static void step_01_handle_grammar_property_start(Context& context_, pmt::util::smrt::GenericAstPath const& path_);
  static void step_01_handle_grammar_property_whitespace(Context& context_, pmt::util::smrt::GenericAstPath const& path_);
  static void step_01_handle_grammar_property_comment(Context& context_, pmt::util::smrt::GenericAstPath const& path_);
  static void step_01_handle_terminal_production(Context& context_, pmt::util::smrt::GenericAstPath const& path_);
  static void step_01_handle_rule_production(Context& context_, pmt::util::smrt::GenericAstPath const& path_);

  static void step_02(Context& context_);
  static void step_03(Context& context_);
  static void step_04(Context& context_);
  static void step_05(Context& context_);
  static void step_06(Context& context_);
  static void step_07(Context& context_);
  static void step_08(Context& context_);
  static void step_09(Context& context_);
  static void step_10(Context& context_);
  static void step_11(Context& context_);
  static void step_12(Context& context_);

  template <pmt::util::smct::IsStateTag TAG_>
  static void write_dot(Context& context_, pmt::util::smct::StateMachine const& state_machine_);

  static auto accepts_to_label(Context& context_, size_t accept_idx_) -> std::string;

  static void sort_terminals_by_name(Context& context_);
  static void sort_rules_by_name(Context& context_);

  static inline const char* const DOT_FILE_PREFIX = "graph_";
  static inline size_t const DOT_FILE_MAX_STATES = 750;
  static inline bool const CASE_SENSITIVITY_DEFAULT = true;
  static inline bool const MERGE_DEFAULT = false;
  static inline bool const UNPACK_DEFAULT = false;
  static inline bool const HIDE_DEFAULT = false;
};

class ParserBuilder::Context {
 public:
  // - Public functions -
  auto try_find_terminal_definition(std::string const& name_) -> pmt::util::smrt::GenericAstPath;
  auto try_find_rule_definition(std::string const& name_) -> pmt::util::smrt::GenericAstPath;

  // - Data -
  pmt::util::smrt::GenericAst::UniqueHandle _ast;

  std::vector<std::string> _terminal_names;
  std::vector<std::string> _terminal_id_names;
  std::vector<std::optional<size_t>> _terminal_accepts;
  pmt::base::Bitset _terminal_case_sensitive_values;
  pmt::base::Bitset _terminal_case_sensitive_present;

  std::vector<size_t> _accepts;

  std::vector<pmt::util::smrt::GenericAstPath> _terminal_definitions;

  std::vector<pmt::util::smrt::GenericAstPath> _comment_open_definitions;
  std::vector<pmt::util::smrt::GenericAstPath> _comment_close_definitions;

  pmt::util::smrt::GenericAstPath _whitespace_definition;

  pmt::util::smct::StateMachine<pmt::util::smct::StateTagFsm> _fsm;
  pmt::util::smct::StateMachine<pmt::util::smct::StateTagPdm> _pdm;

  std::vector<pmt::util::smct::StateMachine<pmt::util::smct::StateTagFsm>> _terminal_fsms;
  std::vector<pmt::util::smct::StateMachine<pmt::util::smct::StateTagFsm>> _comment_open_fsms;
  std::vector<pmt::util::smct::StateMachine<pmt::util::smct::StateTagFsm>> _comment_close_fsms;

  std::vector<std::string> _rule_names;
  std::vector<std::string> _rule_id_names;
  pmt::base::Bitset _rule_merge_values;
  pmt::base::Bitset _rule_unpack_values;
  pmt::base::Bitset _rule_hide_values;

  std::vector<pmt::util::smrt::GenericAstPath> _rule_definitions;

  std::optional<bool> _case_sensitive;
  std::string _start_symbol;
  std::set<pmt::util::smct::Symbol::ValueType> _whitespace;

  size_t _dot_file_count = 0;
};

}  // namespace pmt::parserbuilder

#include "pmt/parserbuilder/parserbuilder-inl.hpp"