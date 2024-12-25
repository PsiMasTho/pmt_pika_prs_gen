#include "pmt/util/parsect/fa_sink_wrapper.hpp"

#include "pmt/util/parsert/generic_tables_base.hpp"

#include <cassert>

namespace pmt::util::parsect {
using namespace pmt::util::parsert;

FaSinkWrapper::FaSinkWrapper(Fa const& fa_)
 : _fa(fa_) {
  refresh();
#ifndef NDEBUG
  // Check that state numbers start from 0 and are contiguous
  for (Fa::StateNrType i = 0; i < _fa._states.size(); ++i) {
    assert(_fa._states.contains(i));
  }
#endif
}

auto FaSinkWrapper::get_state_nr_next(Fa::StateNrType state_nr_, Fa::SymbolType symbol_) const -> Fa::StateNrType {
  if (state_nr_ == _fa._states.size()) {
    return _fa._states.size();
  }

  Fa::Transitions const& transitions = _fa._states.find(state_nr_)->second._transitions;
  auto const itr = transitions._symbol_transitions.find(symbol_);
  return itr != transitions._symbol_transitions.end() ? itr->second : _fa._states.size();
}

auto FaSinkWrapper::get_state_nr_sink() const -> std::optional<Fa::StateNrType> {
  return _has_sink ? std::make_optional(_fa._states.size()) : std::nullopt;
}

auto FaSinkWrapper::get_size() const -> size_t {
  return _has_sink ? _fa._states.size() + 1 : _fa._states.size();
}

void FaSinkWrapper::refresh() {
  _has_sink = false;
  for (auto const& [state_nr, state] : _fa._states) {
    for (Fa::SymbolType symbol = 0; symbol < GenericTablesBase::SYMBOL_EOI; ++symbol) {
      if (!state._transitions._symbol_transitions.contains(symbol)) {
        _has_sink = true;
        return;
      }
    }
  }
}

}  // namespace pmt::util::parsect