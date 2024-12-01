#include "pmt/util/parse/fa_part.hpp"

#include <stack>
#include <unordered_map>

namespace pmt::util::parse {

FaPart::FaPart(Fa::StateNrType incoming_state_nr_)
 : _incoming_state_nr(incoming_state_nr_) {
}

auto FaPart::take() -> FaPart {
  FaPart ret = std::move(*this);
  clear_incoming_state_nr();
  clear_outgoing_transitions();
  return ret;
}

auto FaPart::clone(Fa& fa_) const -> FaPart {
  std::unordered_map<Fa::StateNrType, Fa::StateNrType> visited;  // old -> new
  std::stack<Fa::StateNrType> stack;

  auto const push_and_visit = [&](Fa::StateNrType item_) -> Fa::StateNrType {
    if (auto const itr = visited.find(item_); itr != visited.end()) {
      return itr->second;
    }

    Fa::StateNrType const state_nr_new = fa_.get_unused_state_nr();
    visited.insert_or_assign(item_, state_nr_new);
    stack.push(item_);

    return state_nr_new;
  };

  auto const take = [&]() {
    Fa::StateNrType const item = stack.top();
    stack.pop();
    return item;
  };

  FaPart ret(push_and_visit(*_incoming_state_nr));

  while (!stack.empty()) {
    Fa::StateNrType const state_nr_old = take();
    Fa::StateNrType const state_nr_new = visited.find(state_nr_old)->second;

    Fa::State const& state_old = fa_._states.find(state_nr_old)->second;
    Fa::State& state_new = fa_._states[state_nr_new];

    state_new._accepts = state_old._accepts;

    for (Fa::StateNrType const state_nr_next_old : state_old._transitions._epsilon_transitions) {
      Fa::StateNrType const state_nr_next_new = push_and_visit(state_nr_next_old);
      state_new._transitions._epsilon_transitions.insert(state_nr_next_new);
    }

    for (auto const& [symbol, state_nr_next_old] : state_old._transitions._symbol_transitions) {
      Fa::StateNrType const state_nr_next_new = push_and_visit(state_nr_next_old);
      state_new._transitions._symbol_transitions.insert_or_assign(symbol, state_nr_next_new);
    }

    if (_outgoing_epsilon_transitions.contains(state_nr_old)) {
      ret.add_outgoing_epsilon_transition(state_nr_new);
    }

    if (auto const itr = _outgoing_symbol_transitions.find(state_nr_old); itr != _outgoing_symbol_transitions.end()) {
      for (Fa::SymbolType const symbol : itr->second) {
        ret.add_outgoing_symbol_transition(state_nr_new, symbol);
      }
    }
  }

  return ret;
}

void FaPart::set_incoming_state_nr(Fa::StateNrType incoming_state_nr_) {
  _incoming_state_nr = incoming_state_nr_;
}

auto FaPart::get_incoming_state_nr() const -> std::optional<Fa::StateNrType> {
  return _incoming_state_nr;
}

void FaPart::clear_incoming_state_nr() {
  _incoming_state_nr.reset();
}

void FaPart::add_outgoing_symbol_transition(Fa::StateNrType state_nr_from_, Fa::SymbolType symbol_) {
  _outgoing_symbol_transitions[state_nr_from_].insert(symbol_);
}

void FaPart::add_outgoing_epsilon_transition(Fa::StateNrType state_nr_from_) {
  _outgoing_epsilon_transitions.insert(state_nr_from_);
}

void FaPart::clear_outgoing_transitions() {
  _outgoing_symbol_transitions.clear();
  _outgoing_epsilon_transitions.clear();
}

void FaPart::merge_outgoing_transitions(FaPart& other_) {
  for (auto& [state_nr_from, symbols] : other_._outgoing_symbol_transitions) {
    _outgoing_symbol_transitions[state_nr_from].merge(symbols);
  }

  _outgoing_epsilon_transitions.merge(other_._outgoing_epsilon_transitions);

  other_.clear_outgoing_transitions();
}

void FaPart::connect_outgoing_transitions_to(Fa::StateNrType state_nr_to_, Fa& fa_) {
  // In case it doesn't exist yet
  fa_._states[state_nr_to_];

  for (auto const& [state_nr_from, symbols] : _outgoing_symbol_transitions) {
    for (auto const symbol : symbols) {
      fa_._states[state_nr_from]._transitions._symbol_transitions[symbol] = state_nr_to_;
    }
  }

  for (auto const state_nr_from : _outgoing_epsilon_transitions) {
    fa_._states[state_nr_from]._transitions._epsilon_transitions.insert(state_nr_to_);
  }

  clear_outgoing_transitions();
}

}  // namespace pmt::util::parse