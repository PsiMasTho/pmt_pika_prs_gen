#include "pmt/parserbuilder/lexer_builder.hpp"

#include "pmt/base/dynamic_bitset.hpp"
#include "pmt/base/dynamic_bitset_converter.hpp"
#include "pmt/parserbuilder/fa_part.hpp"
#include "pmt/parserbuilder/fa_to_dsnc_transitions.hpp"
#include "pmt/parserbuilder/grm_ast.hpp"
#include "pmt/parserbuilder/grm_number.hpp"
#include "pmt/util/parse/ast_position.hpp"
#include "pmt/util/parse/fa.hpp"
#include "pmt/util/parse/generic_ast.hpp"
#include "pmt/util/parse/graph_writer.hpp"

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <limits>
#include <stack>
#include <unordered_set>
#include <utility>
#include <vector>

namespace pmt::parserbuilder {
using namespace pmt::util::parse;
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
    std::unordered_set<std::string>& _terminal_stack_contents;
    std::vector<std::string>& _terminal_stack;
  };

  explicit ExpressionFrameBase(AstPositionConst ast_position_);
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
  void process_stage_0(CallstackType& callstack_, Captures& captures_);
  void process_stage_1(CallstackType& callstack_);
  void process_stage_2(CallstackType& callstack_, Captures& captures_);

  FaPart _sub_part;
  Fa::Transitions* _transitions = nullptr;
  size_t _stage = 0;
  size_t _idx = 0;
};

// -- RepetitionExpressionFrame --
class RepetitionExpressionFrame : public ExpressionFrameBase {
 public:
  explicit RepetitionExpressionFrame(AstPositionConst ast_position_);
  void process(CallstackType& callstack_, Captures& captures_) final;

 private:
  void process_stage_0(CallstackType& callstack_, Captures& captures_);
  void process_stage_1(CallstackType& callstack_);
  void process_stage_2(CallstackType& callstack_, Captures& captures_);

  auto is_last() const -> bool;
  auto is_chunk_first() const -> bool;
  auto is_chunk_last() const -> bool;

  FaPart _choices;
  FaPart _chunk;
  GrmNumber::RepetitionRangeType _range;
  Fa::Transitions* _transitions_choices = nullptr;
  size_t _stage = 0;
  size_t _idx = 0;
  size_t _idx_max = 0;
  size_t _idx_chunk = 0;
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

  static auto make_epsilon(Fa& fa_) -> FaPart;
};

class TerminalIdentifierExpressionFrame : public ExpressionFrameBase {
 public:
  using ExpressionFrameBase::ExpressionFrameBase;
  void process(CallstackType& callstack_, Captures& captures_) final;

 private:
  void process_stage_0(CallstackType& callstack_, Captures& captures_);
  void process_stage_1(Captures& captures_);

  std::string const* _terminal_name = nullptr;
  Fa::Transitions* _transitions_reference = nullptr;
  Fa::StateNrType _state_nr_reference = std::numeric_limits<Fa::StateNrType>::max();
  size_t _stage = 0;
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
  _stage = 0;
  ++_idx;

  // If is first
  if (_idx == 1) {
    _sub_part = captures_._ret_part.take();
  } else {
    _sub_part.connect_outgoing_transitions_to(*captures_._ret_part.get_incoming_state_nr(), captures_._result);
    _sub_part.merge_outgoing_transitions(captures_._ret_part);
  }

  // If is last
  if (_idx == _ast_position.first->get_child_at(_ast_position.second)->get_children_size()) {
    captures_._ret_part = _sub_part;
  } else {
    callstack_.push(shared_from_this());
  }
}

// -- ChoicesExpressionFrame --
void ChoicesExpressionFrame::process(CallstackType& callstack_, Captures& captures_) {
  switch (_stage) {
    case 0:
      process_stage_0(callstack_, captures_);
      break;
    case 1:
      process_stage_1(callstack_);
      break;
    case 2:
      process_stage_2(callstack_, captures_);
      break;
  }
}

void ChoicesExpressionFrame::process_stage_0(CallstackType& callstack_, Captures& captures_) {
  Fa::StateNrType state_nr_incoming = captures_._result.get_unused_state_nr();
  _transitions = &captures_._result._states[state_nr_incoming]._transitions;
  _sub_part.set_incoming_state_nr(state_nr_incoming);

  callstack_.push(shared_from_this());
  ++_stage;
}

void ChoicesExpressionFrame::process_stage_1(CallstackType& callstack_) {
  callstack_.push(shared_from_this());
  ++_stage;

  GenericAst const& cur_expr = *_ast_position.first->get_child_at(_ast_position.second);
  callstack_.push(ExpressionFrameFactory::construct(AstPositionConst{&cur_expr, _idx}));
}

void ChoicesExpressionFrame::process_stage_2(CallstackType& callstack_, Captures& captures_) {
  --_stage;
  ++_idx;

  _transitions->_epsilon_transitions.insert(*captures_._ret_part.get_incoming_state_nr());
  _sub_part.merge_outgoing_transitions(captures_._ret_part);

  GenericAst const& cur_expr = *_ast_position.first->get_child_at(_ast_position.second);
  if (_idx < cur_expr.get_children_size()) {
    callstack_.push(shared_from_this());
    return;
  }

  captures_._ret_part = _sub_part;
}

// -- RepetitionExpressionFrame --
RepetitionExpressionFrame::RepetitionExpressionFrame(AstPositionConst ast_position_)
 : ExpressionFrameBase({ast_position_.first->get_child_at(ast_position_.second), 0})
 , _range(GrmNumber::get_repetition_range(*ast_position_.first->get_child_at(ast_position_.second)->get_child_at(1))) {
}

void RepetitionExpressionFrame::process(CallstackType& callstack_, Captures& captures_) {
  switch (_stage) {
    case 0:
      process_stage_0(callstack_, captures_);
      break;
    case 1:
      process_stage_1(callstack_);
      break;
    case 2:
      process_stage_2(callstack_, captures_);
      break;
  }
}

void RepetitionExpressionFrame::process_stage_0(CallstackType& callstack_, Captures& captures_) {
  if (_range.second == 0) {
    captures_._ret_part = EpsilonExpressionFrame::make_epsilon(captures_._result);
    return;
  }

  Fa::StateNrType const state_nr_choices = captures_._result.get_unused_state_nr();
  _transitions_choices = &captures_._result._states[state_nr_choices]._transitions;
  _choices.set_incoming_state_nr(state_nr_choices);

  if (_range.first == 0) {
    FaPart tmp = EpsilonExpressionFrame::make_epsilon(captures_._result);
    _transitions_choices->_epsilon_transitions.insert(*tmp.get_incoming_state_nr());
    _choices.merge_outgoing_transitions(tmp);
    _range.first = 1;
  }

  _idx_max = ((_range.second.value_or(_range.first) - _range.first + 1) * (_range.second.value_or(_range.first) + _range.first)) / 2;

  callstack_.push(shared_from_this());
  ++_stage;
}

void RepetitionExpressionFrame::process_stage_1(CallstackType& callstack_) {
  callstack_.push(shared_from_this());
  ++_stage;

  callstack_.push(ExpressionFrameFactory::construct(_ast_position));
}

void RepetitionExpressionFrame::process_stage_2(CallstackType& callstack_, Captures& captures_) {
  --_stage;

  if (is_chunk_first()) {
    _chunk = captures_._ret_part.take();
  } else {
    _chunk.connect_outgoing_transitions_to(*captures_._ret_part.get_incoming_state_nr(), captures_._result);
    _chunk.merge_outgoing_transitions(captures_._ret_part);
  }

  if (is_chunk_last()) {
    _transitions_choices->_epsilon_transitions.insert(*_chunk.get_incoming_state_nr());
    _choices.merge_outgoing_transitions(_chunk);
    ++_idx_chunk;
  }

  if (!is_last()) {
    callstack_.push(shared_from_this());
  } else {
    if (!_range.second.has_value()) {
      Fa::StateNrType state_nr_loop_back = captures_._result.get_unused_state_nr();
      Fa::State& state_loop_back = captures_._result._states[state_nr_loop_back];
      state_loop_back._transitions._epsilon_transitions.insert(*_choices.get_incoming_state_nr());
      _choices.connect_outgoing_transitions_to(state_nr_loop_back, captures_._result);
      _choices.add_outgoing_epsilon_transition(state_nr_loop_back);
    }

    captures_._ret_part = _choices;
  }

  ++_idx;
}

auto RepetitionExpressionFrame::is_last() const -> bool {
  return _idx == _idx_max - 1;
}

auto RepetitionExpressionFrame::is_chunk_first() const -> bool {
  size_t const idx_chunk_start = (_idx_chunk == 0) ? 0 : _idx_chunk * _range.first + ((_idx_chunk - 1) * _idx_chunk) / 2;
  return _idx == idx_chunk_start;
}

auto RepetitionExpressionFrame::is_chunk_last() const -> bool {
  size_t const idx_chunk_end = (_idx_chunk + 1) * _range.first + (_idx_chunk * (_idx_chunk + 1)) / 2;
  return _idx == idx_chunk_end - 1;
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
  captures_._result._states[state_nr_incoming];
  captures_._ret_part.set_incoming_state_nr(state_nr_incoming);

  GenericAst const& cur_expr = *_ast_position.first->get_child_at(_ast_position.second);

  Fa::SymbolType const min = GrmNumber::single_char_as_value(*cur_expr.get_child_at(0));
  Fa::SymbolType const max = GrmNumber::single_char_as_value(*cur_expr.get_child_at(1));

  for (Fa::SymbolType i = min; i <= max; ++i) {
    captures_._ret_part.add_outgoing_symbol_transition(state_nr_incoming, i);
  }
}

// -- IntegerLiteralExpressionFrame --
void IntegerLiteralExpressionFrame::process(CallstackType&, Captures& captures_) {
  Fa::StateNrType state_nr_incoming = captures_._result.get_unused_state_nr();
  captures_._result._states[state_nr_incoming];
  captures_._ret_part.set_incoming_state_nr(state_nr_incoming);
  captures_._ret_part.add_outgoing_symbol_transition(state_nr_incoming, GrmNumber::single_char_as_value(*_ast_position.first->get_child_at(_ast_position.second)));
}

// -- EpsilonExpressionFrame --
void EpsilonExpressionFrame::process(CallstackType&, Captures& captures_) {
  captures_._ret_part = make_epsilon(captures_._result);
}

auto EpsilonExpressionFrame::make_epsilon(Fa& fa_) -> FaPart {
  Fa::StateNrType state_nr_incoming = fa_.get_unused_state_nr();
  fa_._states[state_nr_incoming];
  FaPart ret(state_nr_incoming);
  ret.add_outgoing_epsilon_transition(state_nr_incoming);
  return ret;
}

// -- TerminalIdentifierExpressionFrame --
void TerminalIdentifierExpressionFrame::process(CallstackType& callstack_, Captures& captures_) {
  switch (_stage) {
    case 0:
      process_stage_0(callstack_, captures_);
      break;
    case 1:
      process_stage_1(captures_);
      break;
  }
}

void TerminalIdentifierExpressionFrame::process_stage_0(CallstackType& callstack_, Captures& captures_) {
  _terminal_name = &_ast_position.first->get_child_at(_ast_position.second)->get_token();

  captures_._terminal_stack.push_back(*_terminal_name);
  if (!captures_._terminal_stack_contents.insert(*_terminal_name).second) {
    std::string msg = "Terminal '" + *_terminal_name + "' is recursive: ";
    std::string delim;
    for (std::string const& terminal : captures_._terminal_stack) {
      msg += std::exchange(delim, " -> ") + terminal;
    }
    throw std::runtime_error(msg);
  }

  ++_stage;
  callstack_.push(shared_from_this());

  if (auto const itr = captures_._terminal_definitions.find(*_terminal_name); itr != captures_._terminal_definitions.end()) {
    _ast_position = itr->second;
  } else {
    throw std::runtime_error("Terminal '" + *_terminal_name + "' not defined");
  }

  _state_nr_reference = captures_._result.get_unused_state_nr();
  _transitions_reference = &captures_._result._states[_state_nr_reference]._transitions;

  callstack_.push(ExpressionFrameFactory::construct(_ast_position));
}

void TerminalIdentifierExpressionFrame::process_stage_1(Captures& captures_) {
  _transitions_reference->_epsilon_transitions.insert(*captures_._ret_part.get_incoming_state_nr());
  captures_._ret_part.set_incoming_state_nr(_state_nr_reference);

  captures_._terminal_stack_contents.erase(*_terminal_name);
  captures_._terminal_stack.pop_back();
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
    case GrmAst::TkTerminalIdentifier:
      return std::make_shared<TerminalIdentifierExpressionFrame>(position_);
    default:
      throw std::runtime_error("Unknown expression frame id: " + GrmAst::to_string(id));
  }
}

}  // namespace

LexerBuilder::LexerBuilder(GenericAst const& ast_, std::set<std::string> const& accepting_terminals_) {
  // Find terminal definitions
  for (size_t i = 0; i < ast_.get_children_size(); ++i) {
    GenericAst const& child = *ast_.get_child_at(i);
    if (child.get_id() == GrmAst::NtTerminalProduction) {
      std::string const& terminal_name = child.get_child_at(0)->get_token();
      std::string const& terminal_id = child.get_child_at(1)->get_token();

      if (accepting_terminals_.contains(terminal_name)) {
        _accepting_terminals.emplace_back(terminal_name);
      }

      _id_names_to_terminal_names[terminal_id].insert(terminal_name);

      if (auto const itr = _terminal_definitions.find(terminal_name); itr == _terminal_definitions.end()) {
        _terminal_definitions.insert_or_assign(terminal_name, AstPositionConst{&child, 2});
      } else {
        throw std::runtime_error("Terminal '" + terminal_name + "' defined multiple times");
      }
    }
  }

  std::ranges::sort(_accepting_terminals);

  // Check if all accepting terminals are defined
  std::unordered_set<std::string const*> missing_terminals;
  for (std::string const& terminal_name : accepting_terminals_) {
    if (!_terminal_definitions.contains(terminal_name)) {
      missing_terminals.insert(&terminal_name);
    }
  }

  if (!missing_terminals.empty()) {
    std::string text = "Missing terminal definition(s) for: ";
    std::string delim;
    for (std::string const* terminal_name : missing_terminals) {
      text += std::exchange(delim, ", ") + *terminal_name;
    }
    throw std::runtime_error(text);
  }

  _terminal_ids.resize(_accepting_terminals.size(), GenericAst::IdDefault);
  for (GenericAst::IdType i = 0; auto const& [terminal_id, terminal_names] : _id_names_to_terminal_names) {
    for (std::string const& terminal_name : terminal_names) {
      std::optional<size_t> const terminal_nr = find_accepting_terminal_nr(terminal_name);
      if (!terminal_nr.has_value()) {
        continue;
      }
      _terminal_ids[*terminal_nr] = (terminal_id == "IdDefault") ? GenericAst::IdDefault : i;
    }
    i += (terminal_id == "IdDefault") ? 0 : 1;
  }
}

auto LexerBuilder::build() -> GenericLexerTables {
  Fa fa = build_initial_fa();
  write_dot(fa);
  fa.determinize();
  write_dot(fa);
  fa.minimize();

  pmt::base::DynamicBitset accepts0 = fa._states.find(0)->second._accepts;
  if (accepts0.any()) {
    static size_t const MAX_REPORTED = 8;
    std::unordered_set<size_t> const accepts0_set = pmt::base::DynamicBitsetConverter::to_unordered_set(accepts0);
    std::string msg;
    std::string delim;
    for (size_t i = 1; size_t const terminal_nr : accepts0_set) {
      msg += std::exchange(delim, ", ");
      if (i <= MAX_REPORTED) {
        msg += _accepting_terminals[terminal_nr];
      } else {
        msg += "...";
        break;
      }
    }
    throw std::runtime_error("Initial state accepts terminal(s): " + msg);
  }

  return fa_to_lexer_tables(fa);
}

auto LexerBuilder::build_initial_fa() -> Fa {
  Fa ret;

  Fa::State& state_start = ret._states[ret.get_unused_state_nr()];

  for (std::string const& terminal_name : _accepting_terminals) {
    AstPositionConst terminal_def = _terminal_definitions.find(terminal_name)->second;

    FaPart ret_part;
    ExpressionFrameBase::CallstackType callstack;
    std::unordered_set<std::string> terminal_stack_contents;
    std::vector<std::string> terminal_stack;
    ExpressionFrameBase::Captures captures{ret_part, ret, _terminal_definitions, terminal_stack_contents, terminal_stack};

    Fa::StateNrType state_nr_terminal_start = ret.get_unused_state_nr();
    Fa::State& state_terminal_start = ret._states[state_nr_terminal_start];
    terminal_stack_contents.insert(terminal_name);
    terminal_stack.push_back(terminal_name);
    state_start._transitions._epsilon_transitions.insert(state_nr_terminal_start);
    callstack.push(ExpressionFrameFactory::construct(terminal_def));

    while (!callstack.empty()) {
      ExpressionFrameBase::FrameHandle cur = callstack.top();
      callstack.pop();
      cur->process(callstack, captures);
    }

    state_terminal_start._transitions._epsilon_transitions.insert(*ret_part.get_incoming_state_nr());

    Fa::StateNrType state_nr_end = ret.get_unused_state_nr();
    Fa::State& state_end = ret._states[state_nr_end];
    state_end._accepts.resize(_accepting_terminals.size(), false);
    state_end._accepts.set(*find_accepting_terminal_nr(terminal_name), true);
    ret_part.connect_outgoing_transitions_to(state_nr_end, ret);
  }

  return ret;
}

auto LexerBuilder::fa_to_lexer_tables(Fa const& fa_) -> GenericLexerTables {
  GenericLexerTables ret;

  // We need to traverse the states in order
  std::set<Fa::StateNrType> state_nrs_sorted;
  for (auto const& [state_nr, state] : fa_._states) {
    state_nrs_sorted.insert(state_nr);
  }

  for (Fa::StateNrType const state_nr : state_nrs_sorted) {
    Fa::State const& state = fa_._states.find(state_nr)->second;
    ret._accepts.emplace_back(pmt::base::DynamicBitset::get_required_chunk_count(_accepting_terminals.size()), 0);
    for (size_t i = 0; i < state._accepts.get_chunk_count(); ++i) {
      ret._accepts.back()[i] = state._accepts.get_chunk(i);
    }
  }

  ret._terminal_names = _accepting_terminals;
  ret._terminal_ids = _terminal_ids;

  for (auto const& [terminal_id, terminal_names] : _id_names_to_terminal_names) {
    if (terminal_id == "IdDefault") {
      continue;
    }
    ret._id_names.push_back(terminal_id);
  }

  create_tables_transitions(fa_, ret);
  return ret;
}

void LexerBuilder::create_tables_transitions(pmt::util::parse::Fa const& fa_, pmt::util::parse::GenericLexerTables& tables_) {
  FaToDsncTransitions fa_to_dsnc_transitions(fa_);
  Dsnc dsnc = fa_to_dsnc_transitions.convert();

  tables_._state_nr_invalid = dsnc._state_nr_invalid;
  tables_._transitions_default = std::move(dsnc._transitions_default);
  tables_._transitions_shift = std::move(dsnc._transitions_shift);
  tables_._transitions_next = std::move(dsnc._transitions_next);
  tables_._transitions_check = std::move(dsnc._transitions_check);
}

auto LexerBuilder::find_accepting_terminal_nr(std::string const& terminal_name_) -> std::optional<size_t> {
  auto const itr = std::lower_bound(_accepting_terminals.begin(), _accepting_terminals.end(), terminal_name_);

  if (itr == _accepting_terminals.end() || *itr != terminal_name_) {
    return std::nullopt;
  }

  return std::distance(_accepting_terminals.begin(), itr);
}

void LexerBuilder::write_dot(Fa const& fa_) {
  if (fa_._states.size() > DOT_FILE_MAX_STATES) {
    std::cerr << "Skipping dot file write, too many states\n";
    return;
  }

  std::ofstream file(DOT_FILE_PREFIX + std::to_string(_dot_file_count++) + ".dot");
  GraphWriter::write_dot(file, fa_, [this](size_t accepts_) { return accepts_to_label(accepts_); });
}

auto LexerBuilder::accepts_to_label(size_t accepts_) -> std::string {
  return _accepting_terminals[accepts_];
}

}  // namespace pmt::parserbuilder