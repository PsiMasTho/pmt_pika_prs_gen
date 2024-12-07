#pragma once

#include "pmt/fw_decl.hpp"
#include "pmt/util/parse/ast_position.hpp"
#include "pmt/util/parse/fa.hpp"
#include "pmt/util/parse/generic_lexer_tables.hpp"
#include "pmt/util/parse/terminal_info.hpp"

#include <map>
#include <optional>
#include <set>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

PMT_FW_DECL_NS_CLASS(pmt::util::parse, GenericAst);

namespace pmt::util::parse {

class LexerBuilder {
 public:
  LexerBuilder(GenericAst const& ast_, std::set<std::string> const& accepting_terminals_);

  auto build() -> GenericLexerTables;

 private:
  auto build_initial_fa() -> Fa;
  auto fa_to_lexer_tables(Fa const& fa_) -> GenericLexerTables;
  auto find_accepting_terminal_nr(std::string const& terminal_name_) -> std::optional<size_t>;
  void write_dot(Fa const& fa_);
  auto accepts_to_label(size_t accepts_) -> std::string;

  static inline const char* const DOT_FILE_PREFIX = "lexer_";
  static inline size_t const DOT_FILE_MAX_STATES = 750;
  std::unordered_map<std::string, AstPositionConst> _terminal_definitions;
  std::map<std::string, std::unordered_set<std::string>> _id_names_to_terminal_names;
  std::vector<TerminalInfo> _accepting_terminals;
  size_t _dot_file_count = 0;
};

}  // namespace pmt::util::parse