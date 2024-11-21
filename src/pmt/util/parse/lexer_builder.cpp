#include "pmt/util/parse/lexer_builder.hpp"

#include "pmt/base/dynamic_bitset.hpp"
#include "pmt/base/dynamic_bitset_converter.hpp"
#include "pmt/util/parse/generic_ast.hpp"
#include "pmt/util/parse/grm_ast.hpp"
#include "pmt/util/parse/grm_ast_transformations.hpp"

#include <iostream>
#include <utility>

namespace pmt::util::parse {

LexerBuilder::LexerBuilder(GenericAst& ast_, std::set<std::string> const& accepting_terminals_)
 : _ast(ast_) {
  // Preprocessing
  try {
    GrmAstTransformations(_ast).transform();
  } catch (std::exception const& e) {
    // clang-format off
    std::cerr 
    << "Exception encountered during preprocessing..\n"
       "Dumping grammar:\n"
       "------------\n";
    // clang-format on
    GrmAstTransformations::emit_grammar(std::cerr, _ast);
    throw;
  }

  // Store accepting terminals
  std::ranges::copy(accepting_terminals_, std::back_inserter(_accepting_terminals));

  pmt::base::DynamicBitset found_accepting_terminals(accepting_terminals_.size());
  // Find terminal definitions
  for (size_t i = 0; i < ast_.get_children_size(); ++i) {
    GenericAst const& child = *ast_.get_child_at(i);
    if (child.get_id() == GrmAst::NtTerminalProduction) {
      std::string const& terminal = child.get_child_at(0)->get_token();

      if (auto const itr = _terminal_definitions.find(terminal); itr == _terminal_definitions.end()) {
        _terminal_definitions.insert_or_assign(terminal, &child);

        if (auto const terminal_nr = find_accepting_terminal_nr(terminal); terminal_nr.has_value()) {
          found_accepting_terminals.set(*terminal_nr, true);
        }
      } else {
        throw std::runtime_error("Terminal '" + terminal + "' defined multiple times");
      }
    }
  }

  // Check if all accepting terminals are defined
  std::set<size_t> const missing_terminals = pmt::base::DynamicBitsetConverter::to_set(found_accepting_terminals.clone_not());
  if (!missing_terminals.empty()) {
    std::string text = "Missing terminal definitions for: ";
    std::string delim;
    for (size_t terminal_nr : missing_terminals) {
      text += std::exchange(delim, ", ") + _accepting_terminals[terminal_nr];
    }
    throw std::runtime_error(text);
  }
}

auto LexerBuilder::build() -> Fa {
  return {};
}

auto LexerBuilder::find_accepting_terminal_nr(std::string const& terminal_) -> std::optional<size_t> {
  auto const itr = std::lower_bound(_accepting_terminals.begin(), _accepting_terminals.end(), terminal_);

  if (itr == _accepting_terminals.end() || *itr != terminal_) {
    return std::nullopt;
  }

  return std::distance(_accepting_terminals.begin(), itr);
}

}  // namespace pmt::util::parse