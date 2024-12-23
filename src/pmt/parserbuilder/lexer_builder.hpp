#pragma once

#include "pmt/base/dynamic_bitset.hpp"
#include "pmt/util/parsect/fa.hpp"
#include "pmt/util/parsert/generic_ast.hpp"
#include "pmt/util/parsert/generic_lexer_tables.hpp"

#include <map>
#include <optional>
#include <string>
#include <unordered_set>
#include <vector>

namespace pmt::parserbuilder {

class LexerBuilderInput {
 public:
};

class LexerBuilder {
 public:
  LexerBuilder(LexerBuilderInput const& input_);

  auto build() -> pmt::util::parsert::GenericLexerTables;

 private:
  auto build_initial_fa() -> pmt::util::parsect::Fa;
  auto fa_to_lexer_tables(pmt::util::parsect::Fa const& fa_) -> pmt::util::parsert::GenericLexerTables;
  auto find_accepting_terminal_nr(std::string const& terminal_name_) -> std::optional<size_t>;
  void write_dot(pmt::util::parsect::Fa const& fa_);
  auto accepts_to_label(size_t accepts_) -> std::string;
};

}  // namespace pmt::parserbuilder