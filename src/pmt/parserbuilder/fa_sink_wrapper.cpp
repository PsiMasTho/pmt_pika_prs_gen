#include "pmt/parserbuilder/fa_sink_wrapper.hpp"

#include <cassert>

namespace pmt::parserbuilder {
using namespace pmt::util::parsect;

FaSinkWrapper::FaSinkWrapper(Fa const& fa_)
 : _fa(fa_) {
#ifndef NDEBUG
  // Check that state numbers start from 0 and are contiguous
  for (Fa::StateNrType i = 0; i < _fa._states.size(); ++i) {
    assert(_fa._states.contains(i));
  }
#endif
}

auto FaSinkWrapper::get_state_nr_next(Fa::StateNrType state_nr_, Fa::SymbolType symbol_) const -> Fa::StateNrType {
  Fa::StateNrType const state_nr_sink = get_state_nr_sink();
  if (state_nr_ == state_nr_sink) {
    return state_nr_sink;
  }

  Fa::Transitions const& transitions = _fa._states.find(state_nr_)->second._transitions;
  auto const itr = transitions._symbol_transitions.find(symbol_);
  return itr != transitions._symbol_transitions.end() ? itr->second : _fa._states.size();
}

auto FaSinkWrapper::get_state_nr_sink() const -> Fa::StateNrType {
  return _fa._states.size();
}

auto FaSinkWrapper::get_size() const -> size_t {
  return _fa._states.size() + 1;
}

}  // namespace pmt::parserbuilder