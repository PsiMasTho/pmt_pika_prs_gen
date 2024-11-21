#pragma once

#include "pmt/fw_decl.hpp"
#include "pmt/util/parse/fa.hpp"

#include <optional>
#include <set>
#include <string>
#include <unordered_map>
#include <vector>

PMT_FW_DECL_NS_CLASS(pmt::util::parse, GenericAst);

namespace pmt::util::parse {

class LexerBuilder {
 public:
  LexerBuilder(GenericAst& ast_, std::set<std::string> const& accepting_terminals_);

  auto build() -> Fa;

 private:
  auto find_accepting_terminal_nr(std::string const& terminal_) -> std::optional<size_t>;

  GenericAst& _ast;
  std::unordered_map<std::string, GenericAst const*> _terminal_definitions;
  std::vector<std::string> _accepting_terminals;
};

}  // namespace pmt::util::parse