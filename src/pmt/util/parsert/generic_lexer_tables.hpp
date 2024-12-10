#pragma once

#include "pmt/util/parsert/generic_id.hpp"

#include <cstdint>
#include <string_view>

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
  TableIndexType _state_nr_min_diff;

  // -- Paddings --
  TableIndexType _padding_l;
  TableIndexType _padding_r;

  // -- Sizes --
  TableIndexType _state_count;
  TableIndexType _compressed_transition_entry_count;
  TableIndexType _accepts_2d_width;

  // -- Transitions --
  StateTransitionEntry const* _state_transition_entries;            // size: _state_count
  CompressedTransitionEntry const* _compressed_transition_entries;  // size: _compressed_transition_entry_count

  // -- Accepts --
  TableIndexType const* _accepts_2d;     // size: _state_count * _accepts_2d_width
  GenericId::IdType const* _accept_ids;  // size: _accepts_2d_width

  // -- Strings --
  std::string_view const* _terminal_names;  // size: _accepts_2d_width
  std::string_view const* _id_names;        // size: (max(_accept_ids) excluding GenericId::*) + 1

  // - Member functions -
  auto get_next_state(TableIndexType state_nr_, TableIndexType symbol_) const -> TableIndexType;
  auto get_id_names_size() const -> size_t;
  auto get_accepts_2d_bitset_at(size_t idx_) const -> GenericLexerTables::TableIndexType const*;
};

}  // namespace pmt::util::parsert