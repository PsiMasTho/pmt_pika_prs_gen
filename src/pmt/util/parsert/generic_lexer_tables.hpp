#pragma once

#include "pmt/base/dynamic_bitset.hpp"
#include "pmt/util/parsert/generic_id.hpp"

#include <cstdint>
#include <string>
#include <vector>

namespace pmt::util::parsert {

class GenericLexerTables {
 public:
  // - Types -
  using TableIndexType = uint64_t;

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
  TableIndexType _accepts_width;

  // -- Transitions --
  std::vector<StateTransitionEntry> _state_transition_entries;            // size: _state_count
  std::vector<CompressedTransitionEntry> _compressed_transition_entries;  // size: _compressed_transition_entry_count

  // -- Accepts --
  std::vector<pmt::base::DynamicBitset> _accepts;  // size: _state_count, all the same width
  std::vector<GenericId::IdType> _accept_ids;      // size: _accepts_width

  // -- Strings --
  std::vector<std::string> _terminal_names;  // size: _accepts_width
  std::vector<std::string> _id_names;        // size: (max(_accept_ids) excluding GenericId::*) + 1

  // - Member functions -
  auto get_state_nr_next(TableIndexType state_nr_, TableIndexType symbol_) const -> TableIndexType;
  auto get_id_names_size() const -> size_t;
};

}  // namespace pmt::util::parsert