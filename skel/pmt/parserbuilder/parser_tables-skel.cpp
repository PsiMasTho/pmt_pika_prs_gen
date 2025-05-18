/* Generated on: /* $replace TIMESTAMP */ */
#include "/* $replace HEADER_INCLUDE_PATH */"

/* $replace NAMESPACE_OPEN */

namespace {
/* $replace PARSER_LOWER_BOUNDS_TYPE */ const PARSER_LOWER_BOUNDS[] = {
/* $replace PARSER_LOWER_BOUNDS */
};

/* $replace PARSER_UPPER_BOUNDS_TYPE */ const PARSER_UPPER_BOUNDS[] = {
/* $replace PARSER_UPPER_BOUNDS */
};

/* $replace PARSER_VALUES_TYPE */ const PARSER_VALUES[] = {
/* $replace PARSER_VALUES */
};

/* $replace PARSER_OFFSETS_TYPE */ const PARSER_OFFSETS[] = {
/* $replace PARSER_OFFSETS */
};

/* $replace LOOKAHEAD_LOWER_BOUNDS_TYPE */ const LOOKAHEAD_LOWER_BOUNDS[] = {
/* $replace LOOKAHEAD_LOWER_BOUNDS */
};

/* $replace LOOKAHEAD_UPPER_BOUNDS_TYPE */ const LOOKAHEAD_UPPER_BOUNDS[] = {
/* $replace LOOKAHEAD_UPPER_BOUNDS */
};

/* $replace LOOKAHEAD_VALUES_TYPE */ const LOOKAHEAD_VALUES[] = {
/* $replace LOOKAHEAD_VALUES */
};

/* $replace LOOKAHEAD_OFFSETS_TYPE */ const LOOKAHEAD_OFFSETS[] = {
/* $replace LOOKAHEAD_OFFSETS */
};

/* $replace PARSER_ACCEPTS_TYPE */ const PARSER_ACCEPTS[] = {
/* $replace PARSER_ACCEPTS */
};

char const* const PARSER_ACCEPTS_LABELS[] = {
/* $replace PARSER_ACCEPTS_LABELS */
};

char const* const ID_NAMES[] = {
/* $replace ID_NAMES */
};

pmt::util::smrt::GenericId::IdType const PARSER_ACCEPTS_IDS[] = {
/* $replace PARSER_ACCEPTS_IDS */
};

pmt::base::Bitset::ChunkType const LOOKAHEAD_ACCEPTS[] = {
/* $replace LOOKAHEAD_ACCEPTS */
};

pmt::base::Bitset::ChunkType const PARSER_UNPACK[] = {
/* $replace PARSER_UNPACK */
};

pmt::base::Bitset::ChunkType const PARSER_HIDE[] = {
/* $replace PARSER_HIDE */
};

pmt::base::Bitset::ChunkType const PARSER_MERGE[] = {
/* $replace PARSER_MERGE */
};

pmt::base::Bitset::ChunkType const PARSER_TERMINAL_TRANSITION_MASKS[] = {
/* $replace PARSER_TERMINAL_TRANSITION_MASKS */
};

pmt::base::Bitset::ChunkType const PARSER_CONFLICT_TRANSITION_MASKS[] = {
/* $replace PARSER_CONFLICT_TRANSITION_MASKS */
};

pmt::base::Bitset::ChunkType const LOOKAHEAD_TERMINAL_TRANSITION_MASKS[] = {
/* $replace LOOKAHEAD_TERMINAL_TRANSITION_MASKS */
};

/* $replace STATE_KINDS_TYPE */ const STATE_KINDS[] = {
/* $replace STATE_KINDS */
};

auto access_bit(std::span<pmt::base::Bitset::ChunkType const> bitset_, size_t index_) -> bool {
 size_t const chunk_index = index_ / pmt::base::Bitset::ChunkBit;
 size_t const bit_index = index_ % pmt::base::Bitset::ChunkBit;
 return (bitset_[chunk_index] & (1 << bit_index)) != 0;
}

auto decode_parser_accept_index(size_t index_) -> size_t {
 return index_ - 1;
}

template <typename LOWER_T_, typename UPPER_T_, typename VALUE_T_, typename OFFSET_T_>
auto get_state_nr_next_generic( std::span<LOWER_T_ const> lowers_, std::span<UPPER_T_ const> uppers_, std::span<VALUE_T_ const> values_, std::span<OFFSET_T_ const> offsets_, pmt::util::smrt::StateNrType state_nr_, pmt::util::smrt::SymbolType symbol_) -> pmt::util::smrt::StateNrType {
 std::span<LOWER_T_ const> lowers_span(&lowers_[offsets_[state_nr_]], offsets_[state_nr_ + 1] - offsets_[state_nr_]);
 std::span<UPPER_T_ const> uppers_span(&uppers_[offsets_[state_nr_]], offsets_[state_nr_ + 1] - offsets_[state_nr_]);

 size_t const idx = std::distance(lowers_span.begin(), std::lower_bound(lowers_span.begin(), lowers_span.end(), symbol_));

 if (idx == lowers_span.size()) {
  if (idx == 0 || symbol_ > uppers_span[idx - 1]) {
   return pmt::util::smrt::StateNrSink;
  }
  return values_[offsets_[state_nr_] + idx - 1];
 }

 if (symbol_ < lowers_span[idx]) {
  if (idx == 0) {
   return pmt::util::smrt::StateNrSink;
  }
  if (symbol_ <= uppers_span[idx - 1]) {
   return values_[offsets_[state_nr_] + idx - 1];
  }
  return pmt::util::smrt::StateNrSink;
 }

 return values_[offsets_[state_nr_] + idx];
}

} // namespace

auto /* $replace CLASS_NAME */::get_state_nr_next(pmt::util::smrt::StateNrType state_nr_, pmt::util::smrt::SymbolType symbol_) const -> pmt::util::smrt::StateNrType {
 return get_state_nr_next_generic</* $replace PARSER_LOWER_BOUNDS_TYPE */, /* $replace PARSER_UPPER_BOUNDS_TYPE */, /* $replace PARSER_VALUES_TYPE */, /* $replace PARSER_OFFSETS_TYPE */>(PARSER_LOWER_BOUNDS, PARSER_UPPER_BOUNDS, PARSER_VALUES, PARSER_OFFSETS, state_nr_, symbol_);
}

auto /* $replace CLASS_NAME */::get_state_terminal_transitions(pmt::util::smrt::StateNrType state_nr_) const -> pmt::base::Bitset::ChunkSpanConst {
 return pmt::base::Bitset::ChunkSpanConst(PARSER_TERMINAL_TRANSITION_MASKS + state_nr_ * /* $replace PARSER_TERMINAL_TRANSITION_MASKS_CHUNK_COUNT */, /* $replace PARSER_TERMINAL_TRANSITION_MASKS_CHUNK_COUNT */);
}

auto /* $replace CLASS_NAME */::get_state_conflict_transitions(pmt::util::smrt::StateNrType state_nr_) const -> pmt::base::Bitset::ChunkSpanConst {
 return pmt::base::Bitset::ChunkSpanConst(PARSER_CONFLICT_TRANSITION_MASKS + state_nr_ * /* $replace PARSER_CONFLICT_TRANSITION_MASKS_CHUNK_COUNT */, /* $replace PARSER_CONFLICT_TRANSITION_MASKS_CHUNK_COUNT */);
}

auto /* $replace CLASS_NAME */::get_state_kind(pmt::util::smrt::StateNrType state_nr_) const -> StateKind {
 return static_cast<StateKind>(STATE_KINDS[state_nr_]);
}

auto /* $replace CLASS_NAME */::get_state_accept_index(pmt::util::smrt::StateNrType state_nr_) const -> size_t {
 return decode_parser_accept_index(PARSER_ACCEPTS[state_nr_]);
}

auto /* $replace CLASS_NAME */::get_eoi_accept_index() const -> size_t {
 return /* $replace EOI_ACCEPT_INDEX */;
}

auto /* $replace CLASS_NAME */::get_accept_count() const -> size_t {
 return /* $replace PARSER_ACCEPT_COUNT */;
}

auto /* $replace CLASS_NAME */::get_conflict_count() const -> size_t {
 return /* $replace PARSER_CONFLICT_COUNT */;
}

auto /* $replace CLASS_NAME */::get_accept_index_label(size_t index_) const -> std::string {
 return PARSER_ACCEPTS_LABELS[index_];
}

auto /* $replace CLASS_NAME */::get_accept_index_unpack(size_t index_) const -> bool {
 return access_bit(PARSER_UNPACK, index_);
}

auto /* $replace CLASS_NAME */::get_accept_index_hide(size_t index_) const -> bool {
 return access_bit(PARSER_HIDE, index_);
}

auto /* $replace CLASS_NAME */::get_accept_index_merge(size_t index_) const -> bool {
 return access_bit(PARSER_MERGE, index_);
}

auto /* $replace CLASS_NAME */::get_accept_index_id(size_t index_) const -> pmt::util::smrt::GenericId::IdType {
 return PARSER_ACCEPTS_IDS[index_];
}

auto /* $replace CLASS_NAME */::id_to_string(pmt::util::smrt::GenericId::IdType id_) const -> std::string {
 return ID_NAMES[id_ - /* $replace MIN_ID */];
}

auto /* $replace CLASS_NAME */::get_min_id() const -> pmt::util::smrt::GenericId::IdType {
 return /* $replace MIN_ID */;
}

auto /* $replace CLASS_NAME */::get_id_count() const -> size_t {
 return /* $replace ID_COUNT */;
}

auto /* $replace CLASS_NAME */::get_lookahead_state_nr_next(pmt::util::smrt::StateNrType state_nr_, pmt::util::smrt::SymbolType symbol_) const -> pmt::util::smrt::StateNrType {
 return get_state_nr_next_generic</* $replace LOOKAHEAD_LOWER_BOUNDS_TYPE */, /* $replace LOOKAHEAD_UPPER_BOUNDS_TYPE */, /* $replace LOOKAHEAD_VALUES_TYPE */, /* $replace LOOKAHEAD_OFFSETS_TYPE */>(LOOKAHEAD_LOWER_BOUNDS, LOOKAHEAD_UPPER_BOUNDS, LOOKAHEAD_VALUES, LOOKAHEAD_OFFSETS, state_nr_, symbol_);
}

auto /* $replace CLASS_NAME */::get_lookahead_state_terminal_transitions(pmt::util::smrt::StateNrType state_nr_) const -> pmt::base::Bitset::ChunkSpanConst {
 return pmt::base::Bitset::ChunkSpanConst(PARSER_TERMINAL_TRANSITION_MASKS + state_nr_ * /* $replace LOOKAHEAD_TERMINAL_TRANSITION_MASKS_CHUNK_COUNT */, /* $replace LOOKAHEAD_TERMINAL_TRANSITION_MASKS_CHUNK_COUNT */);
}

auto /* $replace CLASS_NAME */::get_lookahead_state_accepts(pmt::util::smrt::StateNrType state_nr_) const -> pmt::base::Bitset::ChunkSpanConst {
 return pmt::base::Bitset::ChunkSpanConst(LOOKAHEAD_ACCEPTS + state_nr_ * /* $replace LOOKAHEAD_ACCEPTS_CHUNK_COUNT */, /* $replace LOOKAHEAD_ACCEPTS_CHUNK_COUNT */);
}

/* $replace NAMESPACE_CLOSE */
