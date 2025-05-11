#pragma once

#include "pmt/base/bitset.hpp"
#include "pmt/util/smrt/generic_ast_path.hpp"
#include "pmt/base/overloaded.hpp"

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
  static inline std::string const TERMINAL_DIRECT_PREFIX = std::string(1, TERMINAL_RESERVED_PREFIX_CH) + "direct_";               // NOLINT
  static inline std::string const TERMINAL_COMMENT_OPEN_PREFIX = std::string(1, TERMINAL_RESERVED_PREFIX_CH) + "comment_open_";   // NOLINT
  static inline std::string const TERMINAL_COMMENT_CLOSE_PREFIX = std::string(1, TERMINAL_RESERVED_PREFIX_CH) + "comment_close_"; // NOLINT
  static inline std::string const TERMINAL_LABEL_WHITESPACE = std::string(1, TERMINAL_RESERVED_PREFIX_CH) + "whitespace";         // NOLINT
  static inline std::string const TERMINAL_LABEL_START = std::string(1, TERMINAL_RESERVED_PREFIX_CH) + "start";                   // NOLINT
  static inline std::string const LABEL_EOI = std::string(1, TERMINAL_RESERVED_PREFIX_CH) + "eoi";                                // NOLINT
  static inline std::string const TERMINAL_LABEL_NEWLINE = std::string(1, TERMINAL_RESERVED_PREFIX_CH) + "newline";               // NOLINT

  static inline bool const MERGE_DEFAULT = false;
  static inline bool const UNPACK_DEFAULT = false;
  static inline bool const HIDE_DEFAULT = false;

  static inline bool const CASE_SENSITIVITY_DEFAULT = true;

  struct TerminalData {
    std::string _label;
    std::string _id_name;
    std::optional<size_t> _terminal;
    pmt::util::smrt::GenericAstPath _definition_path;
    bool _case_sensitive : 1;
  };

  struct NonterminalData {
    std::string _label;
    std::string _id_name;
    pmt::util::smrt::GenericAstPath _definition_path;
    bool _merge : 1;
    bool _unpack : 1;
    bool _hide : 1;
  };

  using FetchLabelString = decltype(pmt::base::Overloaded{
   [](TerminalData const& data_) { return data_._label; },
   [](NonterminalData const& data_) { return data_._label; },
   [](std::string const& data_) { return data_; },
   [](std::string_view const& data_) { return std::string(data_); }
  });

  using FetchIdNameString = decltype(pmt::base::Overloaded{
   [](TerminalData const& data_) { return data_._id_name; },
   [](NonterminalData const& data_) { return data_._id_name; },
   [](std::string const& data_) { return data_; },
   [](std::string_view const& data_) { return std::string(data_); }
  });

  // -$ Data $-
  // terminals
  std::vector<TerminalData> _terminals;
  std::vector<size_t> _terminals_reverse;

  std::vector<pmt::util::smrt::GenericAstPath> _comment_open_definitions;
  std::vector<pmt::util::smrt::GenericAstPath> _comment_close_definitions;
  pmt::util::smrt::GenericAstPath _newline_definition;
  pmt::util::smrt::GenericAstPath _whitespace_definition;

  // nonterminals
  std::vector<NonterminalData> _nonterminals;

  // grammar properties
  std::string _start_nonterminal_label;
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
  static void initial_iteration_handle_grammar_property_newline(GrammarData& grammar_data_, pmt::util::smrt::GenericAstPath const& path_);
  static void initial_iteration_handle_terminal_production(GrammarData& grammar_data_, pmt::util::smrt::GenericAst const& ast_, pmt::util::smrt::GenericAstPath const& path_, InitialIterationContext& context_);
  static void initial_iteration_handle_nonterminal_production(GrammarData& grammar_data_, pmt::util::smrt::GenericAst const& ast_, pmt::util::smrt::GenericAstPath const& path_);

  static void add_reserved_terminals(GrammarData& grammar_data_);
  static void add_reserved_nonterminals(GrammarData& grammar_data_);

  static void sort_terminals_by_label(GrammarData& grammar_data_);
  static void sort_nonterminals_by_label(GrammarData& grammar_data_);

  static void check_terminal_uniqueness(GrammarData& grammar_data_);
  static void check_start_nonterminal_label_defined(GrammarData& grammar_data_);

  static void final_iteration(GrammarData& grammar_data_, pmt::util::smrt::GenericAst& ast_);

  auto try_find_terminal_index_by_label(std::string const& label_) -> size_t;
  auto try_find_nonterminal_index_by_label(std::string const& label_) -> size_t;
};

}  // namespace pmt::parserbuilder