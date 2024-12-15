#include "pmt/util/parsert/generic_lexer_tables.hpp"

#include <cassert>
#include <iostream>

namespace pmt::util::parsert {

auto GenericLexerTables::get_next_state(TableIndexType state_nr_, TableIndexType symbol_) const -> TableIndexType {
  TableIndexType offset = _state_transition_entries[state_nr_]._shift + symbol_;

 std::cout << "state_nr_: " << state_nr_ << " symbol_: " << symbol_ << " offset: " << offset << ", "
 "padding_l: " << _padding_l << ", compressed_transition_entry_count: " << _compressed_transition_entry_count << ", "
 "state_nr_min_diff: " << _state_nr_min_diff << std::endl;

  while (true) {
    if (offset < _padding_l || offset >= _compressed_transition_entry_count + _padding_l) {
      if (_state_nr_min_diff == state_nr_) {
        return _state_nr_min_diff;
      }
    } else {
      TableIndexType const offset_adjusted = offset - _padding_l;
      if (_compressed_transition_entries[offset_adjusted]._check == state_nr_) {
        return _compressed_transition_entries[offset_adjusted]._next;
      }
    }

    TableIndexType const state_nr_next = _state_transition_entries[state_nr_]._default;
    if (state_nr_next == state_nr_) {
      return _state_nr_min_diff;
    }
    state_nr_ = state_nr_next;
    offset = _state_transition_entries[state_nr_]._shift + symbol_;
  }
}

auto GenericLexerTables::get_id_names_size() const -> size_t {
  GenericId::IdType max_id = 0;

  for (TableIndexType i = 0; i < _accepts_2d_width; ++i) {
    max_id = (GenericId::is_generic_id(_accept_ids[i])) ? max_id : std::max(max_id, _accept_ids[i]);
  }

  return max_id + 1;
}

auto GenericLexerTables::get_accepts_2d_bitset_at(size_t idx_) const -> GenericLexerTables::TableIndexType const* {
  assert(idx_ < _state_count);
  return _accepts_2d + idx_ * _accepts_2d_width;
}

}  // namespace pmt::util::parsert