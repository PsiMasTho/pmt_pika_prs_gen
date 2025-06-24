#pragma once

#include "pmt/base/overloaded.hpp"
#include "pmt/parser/generic_ast_path.hpp"
#include "pmt/parser/generic_id.hpp"

#include <limits>
#include <map>
#include <string>
#include <vector>

namespace pmt::parser::grammar {

class GrammarData {
public:
 // -$ Types / Constants $-
 enum : size_t {
  LookupIndexAnonymous = std::numeric_limits<size_t>::max(),
 };

 static inline bool const MERGE_DEFAULT = false;
 static inline bool const UNPACK_DEFAULT = false;
 static inline bool const HIDE_DEFAULT = false;

 struct TerminalAcceptData {
  std::string _name;
  std::string _display_name;
  std::string _id_string;
  GenericAstPath _definition_path;
  bool _used : 1 = false;
 };

 struct NonterminalAcceptData {
  std::string _name;
  std::string _display_name;
  std::string _id_string;
  GenericAstPath _definition_path;
  bool _merge : 1 = MERGE_DEFAULT;
  bool _unpack : 1 = UNPACK_DEFAULT;
  bool _hide : 1 = HIDE_DEFAULT;
  bool _used : 1 = false;
 };

 using FetchLabelString = decltype(pmt::base::Overloaded{[](TerminalAcceptData const& data_) { return data_._name; }, [](NonterminalAcceptData const& data_) { return data_._name; }, [](std::string const& data_) { return data_; },
                                                         [](std::string_view const& data_) {
                                                          return std::string(data_);
                                                         }});

 using FetchIdNameString = decltype(pmt::base::Overloaded{[](TerminalAcceptData const& data_) { return data_._id_string; }, [](NonterminalAcceptData const& data_) { return data_._id_string; }, [](std::string const& data_) { return data_; },
                                                          [](std::string_view const& data_) {
                                                           return std::string(data_);
                                                          }});

 // -$ Data $-
 // terminals
 std::vector<TerminalAcceptData> _terminal_accepts;

 std::vector<GenericAstPath> _comment_open_definitions;
 std::vector<GenericAstPath> _comment_close_definitions;
 GenericAstPath _linecount_definition;
 GenericAstPath _whitespace_definition;
 GenericAstPath _start_nonterminal_definition;

 // nonterminals
 std::vector<NonterminalAcceptData> _nonterminal_accepts;

 // non generic ids
 std::map<std::string, GenericId::IdType> _non_generic_ids;

 // -$ Functions $-
 // --$ Other $--
 // modifies the ast_ passed to it!
 static auto construct_from_ast(GenericAst& ast_) -> GrammarData;

 auto lookup_terminal_name_by_index(size_t index_) const -> std::string;
 auto lookup_terminal_index_by_name(std::string_view label_) const -> size_t;
 auto lookup_terminal_definition_by_index(size_t index_) const -> GenericAstPath;
 auto lookup_nonterminal_name_by_index(size_t index_) const -> std::string;
 auto lookup_nonterminal_index_by_name(std::string_view label_) const -> size_t;
 auto lookup_nonterminal_definition_by_index(size_t index_) const -> GenericAstPath;

 auto get_non_generic_ids() const -> std::map<std::string, GenericId::IdType> const&;

private:
 static void initial_traversal(GrammarData& grammar_data_, GenericAst const& ast_);
 static void initial_traversal_handle_grammar_property(GrammarData& grammar_data_, GenericAst const& ast_, GenericAstPath const& path_);
 static void initial_traversal_handle_grammar_property_start(GrammarData& grammar_data_, GenericAst const& ast_, GenericAstPath const& path_);
 static void initial_traversal_handle_grammar_property_whitespace(GrammarData& grammar_data_, GenericAstPath const& path_);
 static void initial_traversal_handle_grammar_property_comment(GrammarData& grammar_data_, GenericAst const& ast_, GenericAstPath const& path_);
 static void initial_traversal_handle_grammar_property_newline(GrammarData& grammar_data_, GenericAstPath const& path_);
 static void initial_traversal_handle_terminal_production(GrammarData& grammar_data_, GenericAst const& ast_, GenericAstPath const& path_);
 static void initial_traversal_handle_nonterminal_production(GrammarData& grammar_data_, GenericAst const& ast_, GenericAstPath const& path_);

 static void add_reserved_terminal_accepts(GrammarData& grammar_data_);
 static void add_reserved_nonterminal_accepts(GrammarData& grammar_data_);

 static void create_eoi_terminal_definition(GrammarData& grammar_data_, GenericAst& ast_);

 static void sort_terminal_accepts_by_name(GrammarData& grammar_data_);
 static void sort_nonterminal_accepts_by_name(GrammarData& grammar_data_);

 static void check_terminal_uniqueness(GrammarData& grammar_data_);
 static void check_nonterminal_uniqueness(GrammarData& grammar_data_);
 static void check_start_nonterminal_name_defined(GrammarData& grammar_data_);

 static void final_traversal(GrammarData& grammar_data_, GenericAst& ast_);

 static void fill_non_generic_ids(GrammarData& grammar_data_);

 auto try_find_terminal_accept_index_by_name(std::string const& label_) -> size_t;
 auto try_find_nonterminal_accept_index_by_name(std::string const& label_) -> size_t;
};

}  // namespace pmt::parser::grammar