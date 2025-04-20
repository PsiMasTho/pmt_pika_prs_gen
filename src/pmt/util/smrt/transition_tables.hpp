#pragma once

#include "pmt/util/smrt/tables_base.hpp"

#include <vector>

namespace pmt::util::smrt {

class TransitionTables : public TablesBase {
 public:
  // -$ Types / Constants $-
  class StateTransitionEntry {
   public:
    IndexType _default;
    IndexType _shift;
  };

  class CompressedTransitionEntry {
   public:
    IndexType _check;
    IndexType _next;
  };

  enum : IndexType {
    StateNrStart = 0
  };

  // -$ Data $-
  std::vector<StateTransitionEntry> _state_transition_entries;            // size: _state_count
  std::vector<CompressedTransitionEntry> _compressed_transition_entries;  // size: _compressed_transition_entry_count

  IndexType _state_nr_sink;
  IndexType _padding_l;
  IndexType _state_count;
  IndexType _symbol_min;
  IndexType _symbol_max;

  // -$ Functions $-
  // --$ Other $--
  auto get_state_nr_next(IndexType state_nr_, IndexType symbol_) const -> IndexType;
};

}  // namespace pmt::util::smrt