#pragma once

#include "pmt/util/smrt/generic_ast_path.hpp"
#include "pmt/base/overloaded.hpp"

#include <string>
#include <vector>
#include <limits>

namespace pmt::parserbuilder {

class GrammarData {
 public:
  // -$ Types / Constants $-
  enum : size_t {
    LookupIndexAnonymous = std::numeric_limits<size_t>::max(),
  };

  static inline char const TERMINAL_RESERVED_PREFIX_CH = '@';
  static inline std::string const TERMINAL_DIRECT_PREFIX = std::string(1, TERMINAL_RESERVED_PREFIX_CH) + "direct_";               // NOLINT
  static inline std::string const TERMINAL_COMMENT_OPEN_PREFIX = std::string(1, TERMINAL_RESERVED_PREFIX_CH) + "comment_open_";   // NOLINT
  static inline std::string const TERMINAL_COMMENT_CLOSE_PREFIX = std::string(1, TERMINAL_RESERVED_PREFIX_CH) + "comment_close_"; // NOLINT
  static inline std::string const TERMINAL_LABEL_WHITESPACE = std::string(1, TERMINAL_RESERVED_PREFIX_CH) + "whitespace";         // NOLINT
  static inline std::string const TERMINAL_LABEL_START = std::string(1, TERMINAL_RESERVED_PREFIX_CH) + "start";                   // NOLINT
  static inline std::string const LABEL_EOI = std::string(1, TERMINAL_RESERVED_PREFIX_CH) + "eoi";                                // NOLINT
  static inline std::string const TERMINAL_LABEL_LINECOUNT = std::string(1, TERMINAL_RESERVED_PREFIX_CH) + "linecount";           // NOLINT

  static inline bool const MERGE_DEFAULT = false;
  static inline bool const UNPACK_DEFAULT = false;
  static inline bool const HIDE_DEFAULT = false;

  struct TerminalAcceptData {
    std::string _label;
    std::string _id_name;
    pmt::util::smrt::GenericAstPath _definition_path;
    bool _hide : 1 = HIDE_DEFAULT;
    bool _accepted : 1 = false;
  };

  struct NonterminalAcceptData {
    std::string _label;
    std::string _id_name;
    pmt::util::smrt::GenericAstPath _definition_path;
    bool _merge : 1 = MERGE_DEFAULT;
    bool _unpack : 1 = UNPACK_DEFAULT;
    bool _hide : 1 = HIDE_DEFAULT;
  };

  using FetchLabelString = decltype(pmt::base::Overloaded{
   [](TerminalAcceptData const& data_) { return data_._label; },
   [](NonterminalAcceptData const& data_) { return data_._label; },
   [](std::string const& data_) { return data_; },
   [](std::string_view const& data_) { return std::string(data_); }
  });

  using FetchIdNameString = decltype(pmt::base::Overloaded{
   [](TerminalAcceptData const& data_) { return data_._id_name; },
   [](NonterminalAcceptData const& data_) { return data_._id_name; },
   [](std::string const& data_) { return data_; },
   [](std::string_view const& data_) { return std::string(data_); }
  });

  // -$ Data $-
  // terminals
  std::vector<TerminalAcceptData> _terminal_accepts;

  std::vector<pmt::util::smrt::GenericAstPath> _comment_open_definitions;
  std::vector<pmt::util::smrt::GenericAstPath> _comment_close_definitions;
  pmt::util::smrt::GenericAstPath _linecount_definition;
  pmt::util::smrt::GenericAstPath _whitespace_definition;
  pmt::util::smrt::GenericAstPath _start_nonterminal_definition;

  // nonterminals
  std::vector<NonterminalAcceptData> _nonterminal_accepts;

  // -$ Functions $-
  // --$ Other $--
  // modifies the ast_ passed to it!
  static auto construct_from_ast(pmt::util::smrt::GenericAst& ast_) -> GrammarData;

  auto lookup_terminal_label_by_index(size_t index_) const -> std::string;
  auto lookup_terminal_index_by_label(std::string_view label_) const -> size_t;
  auto lookup_terminal_definition_by_index(size_t index_) const -> pmt::util::smrt::GenericAstPath;
  auto lookup_nonterminal_label_by_index(size_t index_) const -> std::string;
  auto lookup_nonterminal_index_by_label(std::string_view label_) const -> size_t;
  auto lookup_nonterminal_definition_by_index(size_t index_) const -> pmt::util::smrt::GenericAstPath;

 private:
  static void initial_iteration(GrammarData& grammar_data_, pmt::util::smrt::GenericAst const& ast_);
  static void initial_iteration_handle_grammar_property(GrammarData& grammar_data_, pmt::util::smrt::GenericAst const& ast_, pmt::util::smrt::GenericAstPath const& path_);
  static void initial_iteration_handle_grammar_property_case_sensitive(GrammarData& grammar_data_, pmt::util::smrt::GenericAst const& ast_, pmt::util::smrt::GenericAstPath const& path_);
  static void initial_iteration_handle_grammar_property_start(GrammarData& grammar_data_, pmt::util::smrt::GenericAst const& ast_, pmt::util::smrt::GenericAstPath const& path_);
  static void initial_iteration_handle_grammar_property_whitespace(GrammarData& grammar_data_, pmt::util::smrt::GenericAstPath const& path_);
  static void initial_iteration_handle_grammar_property_comment(GrammarData& grammar_data_, pmt::util::smrt::GenericAst const& ast_, pmt::util::smrt::GenericAstPath const& path_);
  static void initial_iteration_handle_grammar_property_newline(GrammarData& grammar_data_, pmt::util::smrt::GenericAstPath const& path_);
  static void initial_iteration_handle_terminal_production(GrammarData& grammar_data_, pmt::util::smrt::GenericAst const& ast_, pmt::util::smrt::GenericAstPath const& path_);
  static void initial_iteration_handle_nonterminal_production(GrammarData& grammar_data_, pmt::util::smrt::GenericAst const& ast_, pmt::util::smrt::GenericAstPath const& path_);

  static void add_reserved_terminal_accepts(GrammarData& grammar_data_);
  static void add_reserved_nonterminal_accepts(GrammarData& grammar_data_);

  static void sort_terminal_accepts_by_label(GrammarData& grammar_data_);
  static void sort_nonterminal_accepts_by_label(GrammarData& grammar_data_);

  static void check_terminal_uniqueness(GrammarData& grammar_data_);
  static void check_start_nonterminal_label_defined(GrammarData& grammar_data_);

  static void final_iteration(GrammarData& grammar_data_, pmt::util::smrt::GenericAst& ast_);

  auto try_find_terminal_accept_index_by_label(std::string const& label_) -> size_t;
  auto try_find_nonterminal_accept_index_by_label(std::string const& label_) -> size_t;
};

}  // namespace pmt::parserbuilder