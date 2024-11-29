#include "pmt/util/parse/lexer_builder.hpp"

#include "pmt/base/dynamic_bitset.hpp"
#include "pmt/base/dynamic_bitset_converter.hpp"
#include "pmt/util/parse/choices_expression_frame.hpp"
#include "pmt/util/parse/expression_frame_factory.hpp"
#include "pmt/util/parse/fa_part.hpp"
#include "pmt/util/parse/generic_ast.hpp"
#include "pmt/util/parse/grm_ast.hpp"
#include "pmt/util/parse/grm_ast_transformations.hpp"
#include "pmt/util/parse/range_expression_frame.hpp"
#include "pmt/util/parse/sequence_expression_frame.hpp"
#include "pmt/util/parse/string_literal_expression_frame.hpp"

#include <iostream>
#include <unordered_set>
#include <utility>
#include <vector>

namespace pmt::util::parse {
namespace {
using AstPositionConst = std::pair<GenericAst const*, size_t>;
}

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
  Fa ret;

  size_t const state_nr_start = ret.get_unused_state_nr();
  Fa::State& state_start = ret._states[state_nr_start];

  for (auto const& [terminal, terminal_def] : _terminal_definitions) {
    FaPart ret_part;

    ExpressionFrameBase::CallstackType callstack;
    ExpressionFrameBase::Captures captures{ret_part, ret};

    callstack.push(ExpressionFrameFactory::construct({terminal_def, 1}));

    while (!callstack.empty()) {
      ExpressionFrameBase::FrameHandle cur = callstack.top();
      callstack.pop();

      switch (cur->get_id()) {
        case GrmAst::NtSequence:
          static_cast<SequenceExpressionFrame&>(*cur).process(callstack, captures);
          break;
        case GrmAst::NtChoices:
          static_cast<ChoicesExpressionFrame&>(*cur).process(callstack, captures);
          break;
        case GrmAst::TkStringLiteral:
          static_cast<StringLiteralExpressionFrame&>(*cur).process(callstack, captures);
          break;
        case GrmAst::NtRange:
          static_cast<RangeExpressionFrame&>(*cur).process(callstack, captures);
          break;
      }
    }

    state_start._transitions._epsilon_transitions.insert(*ret_part.get_incoming_state_nr());

    Fa::StateNrType state_nr_end = ret.get_unused_state_nr();
    Fa::State& state_end = ret._states[state_nr_end];
    state_end._accepts.resize(_accepting_terminals.size(), false);
    state_end._accepts.set(*find_accepting_terminal_nr(terminal), true);
    ret_part.connect_outgoing_transitions_to(state_nr_end, ret);
  }

  return ret;
}

auto LexerBuilder::find_accepting_terminal_nr(std::string const& terminal_) -> std::optional<size_t> {
  auto const itr = std::lower_bound(_accepting_terminals.begin(), _accepting_terminals.end(), terminal_);

  if (itr == _accepting_terminals.end() || *itr != terminal_) {
    return std::nullopt;
  }

  return std::distance(_accepting_terminals.begin(), itr);
}

}  // namespace pmt::util::parse