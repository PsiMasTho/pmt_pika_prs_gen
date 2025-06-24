#include "pmt/parser/builder/table_writer_common.hpp"

#include "pmt/asserts.hpp"
#include "pmt/parser/rt/util.hpp"
#include "pmt/util/sm/ct/state_machine.hpp"
#include "pmt/util/timestamp.hpp"

#include <map>

namespace pmt::parser::builder {
using namespace pmt::util::sm::ct;
using namespace pmt::util::sm;
using namespace pmt::parser::rt;
using namespace pmt::base;

void TableWriterCommon::replace_array(pmt::util::SkeletonReplacerBase& skeleton_replacer_, std::string& str_, std::string const& label_, std::span<std::string const> const& entries_) {
 std::string replacement_str;

 std::string delim;
 for (size_t i = 0; i < entries_.size(); ++i) {
  if (i == 0) {
   replacement_str += " ";
  } else {
   replacement_str += ",\n ";
  }
  replacement_str += '"' + entries_[i] + '"';
 }

 skeleton_replacer_.replace_skeleton_label(str_, label_, replacement_str);
 skeleton_replacer_.replace_skeleton_label(str_, label_ + "_TYPE", "char const* const");
 skeleton_replacer_.replace_skeleton_label(str_, label_ + "_SIZE", TableWriterCommon::as_hex(entries_.size(), true));
}

void TableWriterCommon::replace_transitions(pmt::util::SkeletonReplacerBase& skeleton_replacer_, std::string& str_, std::string const& prefix_, pmt::util::sm::ct::StateMachine const& state_machine_) {
 IntervalSet<StateNrType> state_nrs = state_machine_.get_state_nrs();
 SymbolKindType kind_max = 0;
 // Build up a map of the transitions
 std::map<SymbolKindType, std::map<StateNrType, std::vector<std::pair<Interval<SymbolValueType>, StateNrType>>>> transitions_map;
 state_nrs.for_each_key([&](StateNrType state_nr_) {
  state_machine_.get_state(state_nr_)->get_symbol_kinds().for_each_key([&](SymbolKindType symbol_kind_) {
   kind_max = std::max(kind_max, symbol_kind_);
   state_machine_.get_state(state_nr_)->get_symbol_transitions(symbol_kind_).for_each_interval([&](StateNrType const state_nr_next_, Interval<SymbolValueType> const interval_) { transitions_map[symbol_kind_][state_nr_].emplace_back(Interval<SymbolValueType>(encode_symbol(interval_.get_lower()), encode_symbol(interval_.get_upper())), state_nr_next_); });
  });
 });

 state_nrs.insert(Interval((state_nrs.empty()) ? 0 : state_nrs.highest() + 1));

 // Add all state nrs to every kind
 for (SymbolKindType kind = 0; kind <= kind_max; ++kind) {
  state_nrs.for_each_key([&](StateNrType state_nr_) { transitions_map[kind][state_nr_]; });
 }

 // Calculate the kinds offsets
 std::vector<size_t> kinds_offsets;
 {
  auto itr_prev = transitions_map.begin();
  for (auto itr_cur = itr_prev; itr_cur != transitions_map.end(); ++itr_cur) {
   if (itr_cur == transitions_map.begin()) {
    kinds_offsets.push_back(0);
    continue;
   } else {
    kinds_offsets.push_back(kinds_offsets.back() + itr_prev->second.size());
   }
   itr_prev = itr_cur;
  }
 }

 // Calculate the states offsets
 std::vector<size_t> states_offsets;
 for (auto const& [kind, transitions] : transitions_map) {
  auto itr_prev = transitions.begin();
  for (auto itr_cur = itr_prev; itr_cur != transitions.end(); ++itr_cur) {
   if (itr_cur == transitions.begin()) {
    states_offsets.push_back((states_offsets.empty()) ? 0 : states_offsets.back());
    continue;
   } else {
    states_offsets.push_back(states_offsets.back() + itr_prev->second.size());
   }
   itr_prev = itr_cur;
  }
 }

 // Flatten transitions_map into a vector
 std::vector<size_t> transitions_flat;
 {
  std::vector<size_t> lowers;
  std::vector<size_t> uppers;
  std::vector<size_t> values;

  for (auto const& [_, transitions] : transitions_map) {
   for (auto const& [state_nr, intervals] : transitions) {
    for (auto const& [interval, state_nr_next] : intervals) {
     lowers.push_back(interval.get_lower());
     uppers.push_back(interval.get_upper());
     values.push_back(state_nr_next);
    }
   }
  }
  // Append into a single vector
  transitions_flat.reserve(lowers.size() + uppers.size() + values.size());
  transitions_flat.insert(transitions_flat.end(), lowers.begin(), lowers.end());
  transitions_flat.insert(transitions_flat.end(), uppers.begin(), uppers.end());
  transitions_flat.insert(transitions_flat.end(), values.begin(), values.end());
 }

 TableWriterCommon::replace_array<size_t>(skeleton_replacer_, str_, prefix_ + "TRANSITIONS", transitions_flat);
 TableWriterCommon::replace_array<size_t>(skeleton_replacer_, str_, prefix_ + "TRANSITIONS_SYMBOL_KIND_OFFSETS", kinds_offsets);
 TableWriterCommon::replace_array<size_t>(skeleton_replacer_, str_, prefix_ + "TRANSITIONS_STATE_OFFSETS", states_offsets);
}

void TableWriterCommon::replace_transition_masks(pmt::util::SkeletonReplacerBase& skeleton_replacer_, std::string& str_, std::string const& label_, StateNrType state_nr_max_, TransitionMaskQueryFn1 const& fn_query_mask_) {
 std::vector<Bitset::ChunkType> terminal_transition_masks_flattened;

 size_t largest_mask = [&] {
  size_t max_size = 0;
  for (StateNrType state_nr_ = 0; state_nr_ < state_nr_max_; ++state_nr_) {
   max_size = std::max(max_size, fn_query_mask_(state_nr_).size());
  }
  return max_size;
 }();

 size_t const chunk_count = Bitset::get_required_chunk_count(largest_mask);

 for (StateNrType state_nr_ = 0; state_nr_ < state_nr_max_; ++state_nr_) {
  Bitset::ChunkSpanConst const chunks = fn_query_mask_(state_nr_);
  for (size_t i = 0; i < chunk_count; ++i) {
   terminal_transition_masks_flattened.push_back((i < chunks.size()) ? chunks[i] : Bitset::ALL_SET_MASKS[false]);
  }
 }

 TableWriterCommon::replace_array<Bitset::ChunkType>(skeleton_replacer_, str_, label_ + "TRANSITION_MASKS", terminal_transition_masks_flattened);
 skeleton_replacer_.replace_skeleton_label(str_, label_ + "TRANSITION_MASKS_CHUNK_COUNT", TableWriterCommon::as_hex(chunk_count, true));
 skeleton_replacer_.replace_skeleton_label(str_, label_ + "TRANSITION_MASKS_SIZE", TableWriterCommon::as_hex(terminal_transition_masks_flattened.size(), true));
}

void TableWriterCommon::replace_timestamp(pmt::util::SkeletonReplacerBase& skeleton_replacer_, std::string& str_) {
 skeleton_replacer_.replace_skeleton_label(str_, "TIMESTAMP", pmt::util::get_timestamp());
}

auto TableWriterCommon::calculate_numeric_entries_per_line(size_t max_width_) -> size_t {
 switch (max_width_) {
  case 0:
   return 0;
  case 1:
   return 25;
  case 2:
   return 22;
  case 3:
   return 20;
  case 4:
   return 18;
  case 5:
   return 17;
  case 6:
   return 15;
  case 7:
   return 14;
  case 8:
   return 13;
  case 9:
   return 12;
  case 10:
   return 11;
  case 11:
   return 11;
  case 12:
   return 10;
  case 13:
   return 10;
  case 14:
   return 9;
  case 15:
   return 9;
  case 16:
   return 8;
  default:
   pmt::unreachable();
 }
}

}  // namespace pmt::parser::builder