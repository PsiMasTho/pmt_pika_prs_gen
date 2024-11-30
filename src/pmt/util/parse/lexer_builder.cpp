#include "pmt/util/parse/lexer_builder.hpp"

#include "pmt/base/dynamic_bitset.hpp"
#include "pmt/base/dynamic_bitset_converter.hpp"
#include "pmt/util/parse/ast_position.hpp"
#include "pmt/util/parse/fa_part.hpp"
#include "pmt/util/parse/generic_ast.hpp"
#include "pmt/util/parse/grm_ast.hpp"
#include "pmt/util/parse/grm_ast_transformations.hpp"

#include <cassert>
#include <cmath>
#include <iostream>
#include <stack>
#include <unordered_set>
#include <utility>
#include <vector>

namespace pmt::util::parse {
namespace {
// - Expression -> Fa declarations -
// -- ExpressionFrameBase --
class ExpressionFrameBase : public std::enable_shared_from_this<ExpressionFrameBase> {
 public:
  using FrameHandle = std::shared_ptr<ExpressionFrameBase>;
  using CallstackType = std::stack<FrameHandle>;

  class Captures {
   public:
    FaPart& _ret_part;
    Fa& _result;
    std::unordered_map<std::string, AstPositionConst> const& _terminal_definitions;
  };

  ExpressionFrameBase(AstPositionConst ast_position_);
  virtual ~ExpressionFrameBase() = default;

  virtual void process(CallstackType& callstack_, Captures& captures_) = 0;

 protected:
  AstPositionConst _ast_position;
};

// -- SequenceExpressionFrame --
class SequenceExpressionFrame : public ExpressionFrameBase {
 public:
  using ExpressionFrameBase::ExpressionFrameBase;
  void process(CallstackType& callstack_, Captures& captures_) final;

 private:
  void process_stage_0(CallstackType& callstack_);
  void process_stage_1(CallstackType& callstack_, Captures& captures_);

  FaPart _sub_part;
  size_t _stage = 0;
  size_t _idx = 0;
};

// -- ChoicesExpressionFrame --
class ChoicesExpressionFrame : public ExpressionFrameBase {
 public:
  using ExpressionFrameBase::ExpressionFrameBase;
  void process(CallstackType& callstack_, Captures& captures_) final;

 private:
  void process_stage_0(CallstackType& callstack_);
  void process_stage_1(CallstackType& callstack_, Captures& captures_);

  std::vector<FaPart> _sub_parts;
  size_t _stage = 0;
};

// -- RepetitionExpressionFrame --
class RepetitionExpressionFrame : public ExpressionFrameBase {
 public:
  RepetitionExpressionFrame(AstPositionConst ast_position_);
  void process(CallstackType& callstack_, Captures& captures_) final;

 private:
  void process_stage_0(CallstackType& callstack_, Captures& captures_);
  void process_stage_1(CallstackType& callstack_, Captures& captures_);
  void process_stage_2(CallstackType& callstack_, Captures& captures_);

  FaPart _sub_part;
  size_t _idx = 0;
  size_t _stage = 0;
  GrmAstTransformations::RepetitionRangeType _range;
};

// -- StringLiteralExpressionFrame --
class StringLiteralExpressionFrame : public ExpressionFrameBase {
 public:
  using ExpressionFrameBase::ExpressionFrameBase;
  void process(CallstackType& callstack_, Captures& captures_) final;
};

// -- RangeExpressionFrame --
class RangeExpressionFrame : public ExpressionFrameBase {
 public:
  using ExpressionFrameBase::ExpressionFrameBase;
  void process(CallstackType& callstack_, Captures& captures_) final;
};

// -- IntegerLiteralExpressionFrame --
class IntegerLiteralExpressionFrame : public ExpressionFrameBase {
 public:
  using ExpressionFrameBase::ExpressionFrameBase;
  void process(CallstackType& callstack_, Captures& captures_) final;
};

// -- EpsilonExpressionFrame --
class EpsilonExpressionFrame : public ExpressionFrameBase {
 public:
  using ExpressionFrameBase::ExpressionFrameBase;
  void process(CallstackType& callstack_, Captures& captures_) final;
};

class TerminalIdentifierExpressionFrame : public ExpressionFrameBase {
 public:
  using ExpressionFrameBase::ExpressionFrameBase;
  void process(CallstackType& callstack_, Captures& captures_) final;
};

// -- ExpressionFrameFactory --
class ExpressionFrameFactory {
 public:
  static auto construct(AstPositionConst position_) -> ExpressionFrameBase::FrameHandle;
};

// - Expression -> Fa definitions -
// -- ExpressionFrameBase --
ExpressionFrameBase::ExpressionFrameBase(AstPositionConst ast_position_)
 : _ast_position(std::move(ast_position_)) {
}

// -- SequenceExpressionFrame --
void SequenceExpressionFrame::process(CallstackType& callstack_, Captures& captures_) {
  switch (_stage) {
    case 0:
      process_stage_0(callstack_);
      break;
    case 1:
      process_stage_1(callstack_, captures_);
      break;
  }
}

void SequenceExpressionFrame::process_stage_0(CallstackType& callstack_) {
  callstack_.push(shared_from_this());
  ++_stage;

  GenericAst const& cur_expr = *_ast_position.first->get_child_at(_ast_position.second);
  callstack_.push(ExpressionFrameFactory::construct(AstPositionConst{&cur_expr, _idx}));
}

void SequenceExpressionFrame::process_stage_1(CallstackType& callstack_, Captures& captures_) {
  if (_idx == 0) {
    _sub_part = captures_._ret_part;
    ++_idx;
  }

  if (_idx > 0 && _idx < _ast_position.first->get_child_at(_ast_position.second)->get_children_size() - 1) {
    _sub_part.connect_outgoing_transitions_to(*captures_._ret_part.get_incoming_state_nr(), captures_._result);
    _sub_part.merge_outgoing_transitions(captures_._ret_part);
    callstack_.push(shared_from_this());
    _stage = 0;
    ++_idx;
    return;
  }

  if (_idx == _ast_position.first->get_child_at(_ast_position.second)->get_children_size() - 1) {
    _sub_part.connect_outgoing_transitions_to(*captures_._ret_part.get_incoming_state_nr(), captures_._result);
    _sub_part.merge_outgoing_transitions(captures_._ret_part);
  }

  captures_._ret_part = _sub_part;
}

// -- ChoicesExpressionFrame --
void ChoicesExpressionFrame::process(CallstackType& callstack_, Captures& captures_) {
  switch (_stage) {
    case 0:
      process_stage_0(callstack_);
      break;
    case 1:
      process_stage_1(callstack_, captures_);
      break;
  }
}

void ChoicesExpressionFrame::process_stage_0(CallstackType& callstack_) {
  callstack_.push(shared_from_this());
  ++_stage;

  GenericAst const& cur_expr = *_ast_position.first->get_child_at(_ast_position.second);
  callstack_.push(ExpressionFrameFactory::construct(AstPositionConst{&cur_expr, _sub_parts.size()}));
}

void ChoicesExpressionFrame::process_stage_1(CallstackType& callstack_, Captures& captures_) {
  GenericAst const& cur_expr = *_ast_position.first->get_child_at(_ast_position.second);

  if (_sub_parts.size() < cur_expr.get_children_size() - 1) {
    _sub_parts.push_back(captures_._ret_part);
    callstack_.push(shared_from_this());
    _stage = 0;
    return;
  }

  _sub_parts.push_back(captures_._ret_part);

  // Create a new incoming state
  Fa::StateNrType state_nr_incoming = captures_._result.get_unused_state_nr();
  Fa::State& state_incoming = captures_._result._states[state_nr_incoming];
  captures_._ret_part.set_incoming_state_nr(state_nr_incoming);

  // Connect with epsilon transitions to the sub-parts
  for (FaPart& part : _sub_parts) {
    state_incoming._transitions._epsilon_transitions.insert(*part.get_incoming_state_nr());
    captures_._ret_part.merge_outgoing_transitions(part);
  }
}

// -- RepetitionExpressionFrame --
RepetitionExpressionFrame::RepetitionExpressionFrame(AstPositionConst ast_position_)
 : ExpressionFrameBase(std::move(ast_position_))
 , _range(GrmAstTransformations::get_repetition_range(*_ast_position.first->get_child_at(_ast_position.second))) {
}

void RepetitionExpressionFrame::process(CallstackType& callstack_, Captures& captures_) {
  switch (_stage) {
    case 0:
      process_stage_0(callstack_, captures_);
      break;
    case 1:
      process_stage_1(callstack_, captures_);
      break;
    case 2:
      process_stage_2(callstack_, captures_);
      break;
  }
}

void RepetitionExpressionFrame::process_stage_0(CallstackType& callstack_, Captures& captures_) {
  Fa::StateNrType state_nr_incoming = captures_._result.get_unused_state_nr();
  captures_._result._states[state_nr_incoming];
  captures_._ret_part.set_incoming_state_nr(state_nr_incoming);

  if (_range.second == 0) {
    captures_._ret_part.add_outgoing_epsilon_transition(state_nr_incoming);
    return;
  }

  callstack_.push(shared_from_this());
  ++_stage;
}

void RepetitionExpressionFrame::process_stage_1(CallstackType& callstack_, Captures& captures_) {
 
}

void RepetitionExpressionFrame::process_stage_2(CallstackType& callstack_, Captures& captures_) {
}

// -- StringLiteralExpressionFrame --
void StringLiteralExpressionFrame::process(CallstackType&, Captures& captures_) {
  // Create a new incoming state
  Fa::StateNrType state_nr_prev = captures_._result.get_unused_state_nr();
  Fa::State* state_prev = &captures_._result._states[state_nr_prev];
  captures_._ret_part.set_incoming_state_nr(state_nr_prev);

  GenericAst const& cur_expr = *_ast_position.first->get_child_at(_ast_position.second);
  for (size_t i = 1; i < cur_expr.get_token().size(); ++i) {
    Fa::StateNrType state_nr_cur = captures_._result.get_unused_state_nr();
    Fa::State* state_cur = &captures_._result._states[state_nr_cur];

    state_prev->_transitions._symbol_transitions[cur_expr.get_token()[i - 1]] = state_nr_cur;
    state_prev = state_cur;
    state_nr_prev = state_nr_cur;
  }

  captures_._ret_part.add_outgoing_symbol_transition(state_nr_prev, cur_expr.get_token().back());
}

// -- RangeExpressionFrame --
void RangeExpressionFrame::process(CallstackType&, Captures& captures_) {
  // Create a new incoming state
  Fa::StateNrType state_nr_incoming = captures_._result.get_unused_state_nr();
  Fa::State& state_incoming = captures_._result._states[state_nr_incoming];
  captures_._ret_part.set_incoming_state_nr(state_nr_incoming);

  GenericAst const& cur_expr = *_ast_position.first->get_child_at(_ast_position.second);

  Fa::SymbolType min = GrmAstTransformations::single_char_as_value(*cur_expr.get_child_at(0));
  Fa::SymbolType max = GrmAstTransformations::single_char_as_value(*cur_expr.get_child_at(1));

  for (Fa::SymbolType i = min; i <= max; ++i) {
    Fa::StateNrType state_nr_next = captures_._result.get_unused_state_nr();
    captures_._result._states[state_nr_next];
    captures_._ret_part.add_outgoing_symbol_transition(state_nr_next, i);
    state_incoming._transitions._epsilon_transitions.insert(state_nr_next);
  }
}

// -- IntegerLiteralExpressionFrame --
void IntegerLiteralExpressionFrame::process(CallstackType&, Captures& captures_) {
  Fa::StateNrType state_nr_incoming = captures_._result.get_unused_state_nr();
  captures_._result._states[state_nr_incoming];
  captures_._ret_part.set_incoming_state_nr(state_nr_incoming);
  captures_._ret_part.add_outgoing_symbol_transition(state_nr_incoming, GrmAstTransformations::single_char_as_value(*_ast_position.first->get_child_at(_ast_position.second)));
}

// -- EpsilonExpressionFrame --
void EpsilonExpressionFrame::process(CallstackType&, Captures& captures_) {
  Fa::StateNrType state_nr_incoming = captures_._result.get_unused_state_nr();
  captures_._result._states[state_nr_incoming];
  captures_._ret_part.set_incoming_state_nr(state_nr_incoming);
  captures_._ret_part.add_outgoing_epsilon_transition(state_nr_incoming);
}

// -- TerminalIdentifierExpressionFrame --
void TerminalIdentifierExpressionFrame::process(CallstackType&, Captures& captures_) {
}

// -- ExpressionFrameFactory --
auto ExpressionFrameFactory::construct(AstPositionConst position_) -> ExpressionFrameBase::FrameHandle {
  GenericAst::IdType const id = position_.first->get_child_at(position_.second)->get_id();
  switch (id) {
    case GrmAst::NtSequence:
      return std::make_shared<SequenceExpressionFrame>(position_);
    case GrmAst::NtChoices:
      return std::make_shared<ChoicesExpressionFrame>(position_);
    case GrmAst::NtRepetition:
      return std::make_shared<RepetitionExpressionFrame>(position_);
    case GrmAst::TkStringLiteral:
      return std::make_shared<StringLiteralExpressionFrame>(position_);
    case GrmAst::NtRange:
      return std::make_shared<RangeExpressionFrame>(position_);
    case GrmAst::TkIntegerLiteral:
      return std::make_shared<IntegerLiteralExpressionFrame>(position_);
    case GrmAst::TkEpsilon:
      return std::make_shared<EpsilonExpressionFrame>(position_);
    default:
      throw std::runtime_error("Unknown expression frame id: " + GrmAst::to_string(id));
  }
}

}  // namespace

LexerBuilder::LexerBuilder(GenericAst& ast_, std::set<std::string> const& accepting_terminals_)
 : _ast(ast_) {
  // Preprocessing
  try {
    // GrmAstTransformations(_ast).transform();
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
        _terminal_definitions.insert_or_assign(terminal, AstPositionConst{&child, 1});

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

  Fa::State& state_start = ret._states[ret.get_unused_state_nr()];

  for (auto const& [terminal, terminal_def] : _terminal_definitions) {
    FaPart ret_part;

    ExpressionFrameBase::CallstackType callstack;
    ExpressionFrameBase::Captures captures{ret_part, ret, _terminal_definitions};

    callstack.push(ExpressionFrameFactory::construct(terminal_def));

    while (!callstack.empty()) {
      ExpressionFrameBase::FrameHandle cur = callstack.top();
      callstack.pop();
      cur->process(callstack, captures);
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