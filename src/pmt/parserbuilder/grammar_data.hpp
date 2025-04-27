#pragma once

#include "pmt/base/bitset.hpp"
#include "pmt/util/smrt/generic_ast_path.hpp"

#include <optional>
#include <string>
#include <vector>

namespace pmt::parserbuilder {

class GrammarData {
  // -$ Types / Constants $-
 private:
  struct InitialIterationContext {
    pmt::base::Bitset _terminal_case_sensitive_present;
    bool _grammar_property_case_sensitive_present : 1 = false;
  };

 public:
  static inline char const TERMINAL_RESERVED_PREFIX_CH = '@';
  static inline std::string const TERMINAL_DIRECT_PREFIX = std::string(1, TERMINAL_RESERVED_PREFIX_CH) + "direct_";  // NOLINT
  static inline std::string const TERMINAL_COMMENT_OPEN_PREFIX = TERMINAL_RESERVED_PREFIX_CH + "comment_open_";      // NOLINT
  static inline std::string const TERMINAL_COMMENT_CLOSE_PREFIX = TERMINAL_RESERVED_PREFIX_CH + "comment_close_";    // NOLINT
  static inline std::string const TERMINAL_NAME_WHITESPACE = TERMINAL_RESERVED_PREFIX_CH + "whitespace";             // NOLINT
  static inline std::string const TERMINAL_NAME_START = TERMINAL_RESERVED_PREFIX_CH + "start";                       // NOLINT
  static inline std::string const TERMINAL_NAME_EOI = TERMINAL_RESERVED_PREFIX_CH + "eoi";                           // NOLINT

  static inline bool const MERGE_DEFAULT = false;
  static inline bool const UNPACK_DEFAULT = false;
  static inline bool const HIDE_DEFAULT = false;

  static inline bool const CASE_SENSITIVITY_DEFAULT = true;

  struct TerminalData {
    std::string _name;
    std::string _id_name;
    std::optional<size_t> _accepts;
    pmt::util::smrt::GenericAstPath _definition_path;
    bool _case_sensitive : 1;
  };

  struct RuleData {
    std::string _name;
    std::string _id_name;
    pmt::util::smrt::GenericAstPath _definition_path;
    bool _merge : 1;
    bool _unpack : 1;
    bool _hide : 1;
  };

  // -$ Data $-
  // terminals
  std::vector<TerminalData> _terminals;
  std::vector<size_t> _accepts_reverse;

  std::vector<pmt::util::smrt::GenericAstPath> _comment_open_definitions;
  std::vector<pmt::util::smrt::GenericAstPath> _comment_close_definitions;
  pmt::util::smrt::GenericAstPath _whitespace_definition;

  // rules
  std::vector<RuleData> _rules;

  // grammar properties
  std::string _start_rule_name;
  bool _case_sensitive : 1 = CASE_SENSITIVITY_DEFAULT;

  // -$ Functions $-
  // --$ Other $--
  // modifies the ast_ passed to it!
  static auto construct_from_ast(pmt::util::smrt::GenericAst& ast_) -> GrammarData;

 private:
  static void initial_iteration(GrammarData& grammar_data_, pmt::util::smrt::GenericAst const& ast_);
  static void initial_iteration_handle_grammar_property(GrammarData& grammar_data_, pmt::util::smrt::GenericAst const& ast_, pmt::util::smrt::GenericAstPath const& path_, InitialIterationContext& context_);
  static void initial_iteration_handle_grammar_property_case_sensitive(GrammarData& grammar_data_, pmt::util::smrt::GenericAst const& ast_, pmt::util::smrt::GenericAstPath const& path_, InitialIterationContext& context_);
  static void initial_iteration_handle_grammar_property_start(GrammarData& grammar_data_, pmt::util::smrt::GenericAst const& ast_, pmt::util::smrt::GenericAstPath const& path_);
  static void initial_iteration_handle_grammar_property_whitespace(GrammarData& grammar_data_, pmt::util::smrt::GenericAstPath const& path_);
  static void initial_iteration_handle_grammar_property_comment(GrammarData& grammar_data_, pmt::util::smrt::GenericAst const& ast_, pmt::util::smrt::GenericAstPath const& path_);
  static void initial_iteration_handle_terminal_production(GrammarData& grammar_data_, pmt::util::smrt::GenericAst const& ast_, pmt::util::smrt::GenericAstPath const& path_, InitialIterationContext& context_);
  static void initial_iteration_handle_rule_production(GrammarData& grammar_data_, pmt::util::smrt::GenericAst const& ast_, pmt::util::smrt::GenericAstPath const& path_);

  static void add_start_and_eoi_terminals(GrammarData& grammar_data_);

  static void sort_terminals_by_name(GrammarData& grammar_data_);
  static void sort_rules_by_name(GrammarData& grammar_data_);

  static void check_terminal_uniqueness(GrammarData& grammar_data_);
  static void check_start_rule_name_defined(GrammarData& grammar_data_);

  static void final_iteration(GrammarData& grammar_data_, pmt::util::smrt::GenericAst& ast_);

  auto try_find_terminal_index_by_name(std::string const& name_) -> size_t;
  auto try_find_rule_index_by_name(std::string const& name_) -> size_t;
};

}  // namespace pmt::parserbuilder