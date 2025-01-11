#include "pmt/util/parsert/transition_tables.hpp"

#include <cassert>

namespace pmt::util::parsert {

auto TransitionTables::get_state_nr_next(IndexType state_nr_, IndexType symbol_) const -> IndexType {
  if (symbol_ < _symbol_min || symbol_ > _symbol_max) {
    return _state_nr_sink;
  }

  IndexType offset = _state_transition_entries[state_nr_]._shift + symbol_;

  while (true) {
    if (offset < _padding_l || offset >= _compressed_transition_entries.size() + _padding_l) {
      if (_state_nr_sink == state_nr_) {
        return _state_nr_sink;
      }
    } else {
      IndexType const offset_adjusted = offset - _padding_l;
      if (_compressed_transition_entries[offset_adjusted]._check == state_nr_) {
        return _compressed_transition_entries[offset_adjusted]._next;
      }
    }

    IndexType const state_nr_next = _state_transition_entries[state_nr_]._default;
    if (state_nr_next == state_nr_) {
      return _state_nr_sink;
    }
    state_nr_ = state_nr_next;
    offset = _state_transition_entries[state_nr_]._shift + symbol_;
  }
}

}  // namespace pmt::util::parsert
