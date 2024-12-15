#pragma once

#include "pmt/util/parsect/fa.hpp"
#include "pmt/util/parsert/generic_ast.hpp"
#include "pmt/util/parsert/generic_lexer_tables.hpp"

#include <map>
#include <optional>
#include <set>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace pmt::parserbuilder {

class LexerBuilder {
 public:
  LexerBuilder(pmt::util::parsert::GenericAst const& ast_, std::set<std::string> const& accepting_terminals_);

  auto build() -> pmt::util::parsert::GenericLexerTables;

 private:
  auto build_initial_fa() -> pmt::util::parsect::Fa;
  auto fa_to_lexer_tables(pmt::util::parsect::Fa const& fa_) -> pmt::util::parsert::GenericLexerTables;
  void create_tables_transitions(pmt::util::parsect::Fa const& fa_, pmt::util::parsert::GenericLexerTables& tables_);
  auto find_accepting_terminal_nr(std::string const& terminal_name_) -> std::optional<size_t>;
  void write_dot(pmt::util::parsect::Fa const& fa_);
  auto accepts_to_label(size_t accepts_) -> std::string;

  static inline const char* const DOT_FILE_PREFIX = "lexer_";
  static inline size_t const DOT_FILE_MAX_STATES = 750;

  std::unordered_map<std::string, pmt::util::parsert::GenericAst::PositionConst> _terminal_definitions;
  std::map<std::string, std::unordered_set<std::string>> _id_names_to_terminal_names;
  std::vector<std::string> _accepting_terminals;
  std::vector<pmt::util::parsert::GenericId::IdType> _terminal_ids;
  size_t _dot_file_count = 0;
};

}  // namespace pmt::parserbuilder