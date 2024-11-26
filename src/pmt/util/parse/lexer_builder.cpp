#include "pmt/util/parse/lexer_builder.hpp"

#include "pmt/base/dynamic_bitset.hpp"
#include "pmt/base/dynamic_bitset_converter.hpp"
#include "pmt/util/parse/fa_part.hpp"
#include "pmt/util/parse/generic_ast.hpp"
#include "pmt/util/parse/grm_ast.hpp"
#include "pmt/util/parse/grm_ast_transformations.hpp"

#include <iostream>
#include <limits>
#include <memory>
#include <stack>
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
    struct StackItemBody {
      std::vector<FaPart> _sub_parts;
      AstPositionConst _ast_position;
      // FaPart _state_nr_local;
      size_t _stage;
    };

    using StackItem = std::shared_ptr<StackItemBody>;

    FaPart ret_part;

    std::stack<StackItem> stack;

    auto take = [&stack]() -> StackItem {
      StackItem ret = stack.top();
      stack.pop();
      return ret;
    };

    {
      StackItemBody body;
      body._ast_position = {terminal_def, 1};
      // body._state_nr_local = state_nr_start;
      body._stage = 0;
      stack.push(std::make_shared<StackItemBody>(std::move(body)));
    }

    while (!stack.empty()) {
      StackItem cur = take();
      GenericAst const& expr = *cur->_ast_position.first->get_child_at(cur->_ast_position.second);

      switch (expr.get_id()) {
        case GrmAst::NtSequence: {
          switch (cur->_stage) {
            case 0: {
              ++cur->_stage;
              stack.push(cur);
              StackItem next = std::make_shared<StackItemBody>();
              next->_ast_position = {&expr, cur->_sub_parts.size()};
              next->_stage = 0;
              stack.push(next);
            } break;
            case 1: {
              if (cur->_sub_parts.size() < expr.get_children_size() - 1) {
                cur->_sub_parts.push_back(ret_part);
                cur->_stage = 0;
                stack.push(cur);
              } else {
                cur->_sub_parts.push_back(ret_part);
                cur->_stage = 2;
                stack.push(cur);
              }
            } break;
            case 2: {
              ret_part.set_incoming_state_nr(*cur->_sub_parts.front().get_incoming_state_nr());
              ret_part.merge_outgoing_transitions(cur->_sub_parts.back());

              // Concatenate sub-parts into the return part
              FaPart* prev = &cur->_sub_parts.front();
              for (size_t i = 1; i < cur->_sub_parts.size(); ++i) {
                FaPart* cur_part = &cur->_sub_parts[i];
                prev->connect_outgoing_transitions_to(*cur_part->get_incoming_state_nr(), ret);
              }
            }
          }
          break;
          case GrmAst::NtChoices: {
            switch (cur->_stage) {
              case 0: {
                ++cur->_stage;
                stack.push(cur);
                StackItem next = std::make_shared<StackItemBody>();
                next->_ast_position = {&expr, cur->_sub_parts.size()};
                next->_stage = 0;
                stack.push(next);
              } break;
              case 1: {
                if (cur->_sub_parts.size() < expr.get_children_size() - 1) {
                  cur->_sub_parts.push_back(ret_part);
                  cur->_stage = 0;
                  stack.push(cur);
                } else {
                  cur->_sub_parts.push_back(ret_part);
                  cur->_stage = 2;
                  stack.push(cur);
                }
              } break;
              case 2: {
                // Create a new incoming state
                Fa::StateNrType state_nr_incoming = ret.get_unused_state_nr();
                Fa::State& state_incoming = ret._states[state_nr_incoming];
                ret_part.set_incoming_state_nr(state_nr_incoming);

                // Connect with epsilon transitions to the sub-parts
                for (FaPart& part : cur->_sub_parts) {
                  state_incoming._transitions._epsilon_transitions.insert(*part.get_incoming_state_nr());
                  ret_part.merge_outgoing_transitions(part);
                }
              }
            }
          } break;
          case GrmAst::TkStringLiteral: {
            // Create a new incoming state
            Fa::StateNrType state_nr_prev = ret.get_unused_state_nr();
            Fa::State* state_prev = &ret._states[state_nr_prev];
            ret_part.set_incoming_state_nr(state_nr_prev);

            for (size_t i = 1; i < expr.get_token().size(); ++i) {
              Fa::StateNrType state_nr_cur = ret.get_unused_state_nr();
              Fa::State* state_cur = &ret._states[state_nr_cur];

              state_prev->_transitions._symbol_transitions[expr.get_token()[i - 1]] = state_nr_cur;
              state_prev = state_cur;
              state_nr_prev = state_nr_cur;
            }

            ret_part.add_outgoing_symbol_transition(state_nr_prev, expr.get_token().back());

          } break;
        }
      }
    }

    state_start._transitions._epsilon_transitions.insert(*ret_part.get_incoming_state_nr());

    Fa::StateNrType state_nr_end = ret.get_unused_state_nr();
    Fa::State& state_end = ret._states[state_nr_end];
    state_end._accepts.resize(_accepting_terminals.size(), false);
    state_end._accepts.set(find_accepting_terminal_nr(terminal).value(), true);
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