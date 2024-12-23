#pragma once

#include <vector>

#include "pmt/util/parsert/generic_tables_base.hpp"

namespace pmt::util::parsert {

class GenericFaStateTables : public GenericTablesBase {
 public:
  // - Types -
  class StateTransitionEntry {
   public:
    TableIndexType _default;
    TableIndexType _shift;
  };

  class CompressedTransitionEntry {
   public:
    TableIndexType _check;
    TableIndexType _next;
  };

  // - Static data -
  // -- Special state numbers --
  enum : TableIndexType {
    StateNrStart = 0
  };

  // - Nonstatic data -
  // -- Special state numbers --
  TableIndexType _state_nr_sink;

  // -- Paddings --
  TableIndexType _padding_l;

  // -- Sizes --
  TableIndexType _state_count;

  // -- Transitions --
  std::vector<StateTransitionEntry> _state_transition_entries;            // size: _state_count
  std::vector<CompressedTransitionEntry> _compressed_transition_entries;  // size: _compressed_transition_entry_count

  // - Member functions -
  auto get_state_nr_next(TableIndexType state_nr_, TableIndexType symbol_) const -> TableIndexType;
};

}  // namespace pmt::util::parsert