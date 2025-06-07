/* Generated on: /* $replace TIMESTAMP */ */
#include "/* $replace HEADER_INCLUDE_PATH */"

#include <pmt/util/smrt/util.hpp>

#include <array>

/* $replace NAMESPACE_OPEN */

namespace {
std::array</* $replace PARSER_TRANSITIONS_TYPE */, /* $replace PARSER_TRANSITIONS_SIZE */> const PARSER_TRANSITIONS = {
/* $replace PARSER_TRANSITIONS */
};

std::array</* $replace PARSER_TRANSITIONS_STATE_OFFSETS_TYPE */, /* $replace PARSER_TRANSITIONS_STATE_OFFSETS_SIZE */> const PARSER_TRANSITIONS_STATE_OFFSETS = {
/* $replace PARSER_TRANSITIONS_STATE_OFFSETS */
};

std::array</* $replace PARSER_TRANSITIONS_SYMBOL_KIND_OFFSETS_TYPE */, /* $replace PARSER_TRANSITIONS_SYMBOL_KIND_OFFSETS_SIZE */> const PARSER_TRANSITIONS_SYMBOL_KIND_OFFSETS = {
/* $replace PARSER_TRANSITIONS_SYMBOL_KIND_OFFSETS */
};

std::array</* $replace LOOKAHEAD_TRANSITIONS_TYPE */, /* $replace LOOKAHEAD_TRANSITIONS_SIZE */> const LOOKAHEAD_TRANSITIONS = {
/* $replace LOOKAHEAD_TRANSITIONS */
};

std::array</* $replace LOOKAHEAD_TRANSITIONS_STATE_OFFSETS_TYPE */, /* $replace LOOKAHEAD_TRANSITIONS_STATE_OFFSETS_SIZE */> const LOOKAHEAD_TRANSITIONS_STATE_OFFSETS = {
/* $replace LOOKAHEAD_TRANSITIONS_STATE_OFFSETS */
};

std::array</* $replace LOOKAHEAD_TRANSITIONS_SYMBOL_KIND_OFFSETS_TYPE */, /* $replace LOOKAHEAD_TRANSITIONS_SYMBOL_KIND_OFFSETS_SIZE */> const LOOKAHEAD_TRANSITIONS_SYMBOL_KIND_OFFSETS = {
/* $replace LOOKAHEAD_TRANSITIONS_SYMBOL_KIND_OFFSETS */
};

std::array</* $replace PARSER_ACCEPTS_TYPE */, /* $replace PARSER_ACCEPTS_SIZE */> const PARSER_ACCEPTS = {
/* $replace PARSER_ACCEPTS */
};

std::array</* $replace PARSER_ACCEPT_LABELS_TYPE */, /* $replace PARSER_ACCEPT_LABELS_SIZE */> const PARSER_ACCEPT_LABELS = {
/* $replace PARSER_ACCEPT_LABELS */
};

std::array</* $replace ID_NAMES_TYPE */, /* $replace ID_NAMES_SIZE */> const ID_NAMES = {
/* $replace ID_NAMES */
};

std::array<pmt::util::smrt::GenericId::IdType const, /* $replace PARSER_ACCEPT_IDS_SIZE */> const PARSER_ACCEPT_IDS = {
/* $replace PARSER_ACCEPT_IDS */
};

std::array<pmt::base::Bitset::ChunkType const, /* $replace LOOKAHEAD_ACCEPTS_SIZE */> const LOOKAHEAD_ACCEPTS = {
/* $replace LOOKAHEAD_ACCEPTS */
};

std::array<pmt::base::Bitset::ChunkType const, /* $replace PARSER_UNPACK_SIZE*/> const PARSER_UNPACK = {
/* $replace PARSER_UNPACK */
};

std::array<pmt::base::Bitset::ChunkType const, /* $replace PARSER_HIDE_SIZE */> const PARSER_HIDE = {
/* $replace PARSER_HIDE */
};

std::array<pmt::base::Bitset::ChunkType const, /* $replace PARSER_MERGE_SIZE */> const PARSER_MERGE = {
/* $replace PARSER_MERGE */
};

std::array<pmt::base::Bitset::ChunkType const,  /* $replace PARSER_TERMINAL_TRANSITION_MASKS_SIZE */> const PARSER_TERMINAL_TRANSITION_MASKS = {
/* $replace PARSER_TERMINAL_TRANSITION_MASKS */
};

std::array<pmt::base::Bitset::ChunkType const,  /* $replace PARSER_HIDDEN_TERMINAL_TRANSITION_MASKS_SIZE */> const PARSER_HIDDEN_TERMINAL_TRANSITION_MASKS = {
/* $replace PARSER_HIDDEN_TERMINAL_TRANSITION_MASKS */
};

std::array<pmt::base::Bitset::ChunkType const,  /* $replace PARSER_CONFLICT_TRANSITION_MASKS_SIZE */> const PARSER_CONFLICT_TRANSITION_MASKS = {
/* $replace PARSER_CONFLICT_TRANSITION_MASKS */
};

std::array<pmt::base::Bitset::ChunkType const, /* $replace LOOKAHEAD_TERMINAL_TRANSITION_MASKS_SIZE */> const LOOKAHEAD_TERMINAL_TRANSITION_MASKS = {
/* $replace LOOKAHEAD_TERMINAL_TRANSITION_MASKS */
};

std::array</* $replace PARSER_STATE_KINDS_TYPE */, /* $replace PARSER_STATE_KINDS_SIZE */> const PARSER_STATE_KINDS = {
/* $replace PARSER_STATE_KINDS */
};

} // namespace

auto /* $replace CLASS_NAME */::get_state_nr_next(pmt::util::smrt::StateNrType state_nr_, pmt::util::smrt::SymbolKindType kind_, pmt::util::smrt::SymbolValueType symbol_) const -> pmt::util::smrt::StateNrType {
 return pmt::util::smrt::get_state_nr_next_generic(PARSER_TRANSITIONS, PARSER_TRANSITIONS_STATE_OFFSETS, PARSER_TRANSITIONS_SYMBOL_KIND_OFFSETS, kind_, state_nr_, symbol_);
}

auto /* $replace CLASS_NAME */::get_state_terminal_transitions(pmt::util::smrt::StateNrType state_nr_) const -> pmt::base::Bitset::ChunkSpanConst {
 return pmt::base::Bitset::ChunkSpanConst(PARSER_TERMINAL_TRANSITION_MASKS.begin() + state_nr_ * /* $replace PARSER_TERMINAL_TRANSITION_MASKS_CHUNK_COUNT */, /* $replace PARSER_TERMINAL_TRANSITION_MASKS_CHUNK_COUNT */);
}

auto /* $replace CLASS_NAME */::get_state_hidden_terminal_transitions(pmt::util::smrt::StateNrType state_nr_) const -> pmt::base::Bitset::ChunkSpanConst {
 return pmt::base::Bitset::ChunkSpanConst(PARSER_HIDDEN_TERMINAL_TRANSITION_MASKS.begin() + state_nr_ * /* $replace PARSER_HIDDEN_TERMINAL_TRANSITION_MASKS_CHUNK_COUNT */, /* $replace PARSER_HIDDEN_TERMINAL_TRANSITION_MASKS_CHUNK_COUNT */);
}

auto /* $replace CLASS_NAME */::get_state_conflict_transitions(pmt::util::smrt::StateNrType state_nr_) const -> pmt::base::Bitset::ChunkSpanConst {
 return pmt::base::Bitset::ChunkSpanConst(PARSER_CONFLICT_TRANSITION_MASKS.begin() + state_nr_ * /* $replace PARSER_CONFLICT_TRANSITION_MASKS_CHUNK_COUNT */, /* $replace PARSER_CONFLICT_TRANSITION_MASKS_CHUNK_COUNT */);
}

auto /* $replace CLASS_NAME */::get_state_kind(pmt::util::smrt::StateNrType state_nr_) const -> StateKind {
 return static_cast<StateKind>(PARSER_STATE_KINDS[state_nr_]);
}

auto /* $replace CLASS_NAME */::get_state_accept_index(pmt::util::smrt::StateNrType state_nr_) const -> size_t {
 return pmt::util::smrt::decode_accept_index(PARSER_ACCEPTS[state_nr_]);
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
 return PARSER_ACCEPT_LABELS[index_];
}

auto /* $replace CLASS_NAME */::get_accept_index_unpack(size_t index_) const -> bool {
 return pmt::util::smrt::get_bit(PARSER_UNPACK, index_);
}

auto /* $replace CLASS_NAME */::get_accept_index_hide(size_t index_) const -> bool {
 return pmt::util::smrt::get_bit(PARSER_HIDE, index_);
}

auto /* $replace CLASS_NAME */::get_accept_index_merge(size_t index_) const -> bool {
 return pmt::util::smrt::get_bit(PARSER_MERGE, index_);
}

auto /* $replace CLASS_NAME */::get_accept_index_id(size_t index_) const -> pmt::util::smrt::GenericId::IdType {
 return PARSER_ACCEPT_IDS[index_];
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

auto /* $replace CLASS_NAME */::get_lookahead_state_nr_next(pmt::util::smrt::StateNrType state_nr_, pmt::util::smrt::SymbolValueType symbol_) const -> pmt::util::smrt::StateNrType {
 return pmt::util::smrt::get_state_nr_next_generic(LOOKAHEAD_TRANSITIONS, LOOKAHEAD_TRANSITIONS_STATE_OFFSETS, LOOKAHEAD_TRANSITIONS_SYMBOL_KIND_OFFSETS, pmt::util::smrt::SymbolKindCharacter, state_nr_, symbol_);
}

auto /* $replace CLASS_NAME */::get_lookahead_state_terminal_transitions(pmt::util::smrt::StateNrType state_nr_) const -> pmt::base::Bitset::ChunkSpanConst {
 return pmt::base::Bitset::ChunkSpanConst(PARSER_TERMINAL_TRANSITION_MASKS.begin() + state_nr_ * /* $replace LOOKAHEAD_TERMINAL_TRANSITION_MASKS_CHUNK_COUNT */, /* $replace LOOKAHEAD_TERMINAL_TRANSITION_MASKS_CHUNK_COUNT */);
}

auto /* $replace CLASS_NAME */::get_lookahead_state_accepts(pmt::util::smrt::StateNrType state_nr_) const -> pmt::base::Bitset::ChunkSpanConst {
 return pmt::base::Bitset::ChunkSpanConst(LOOKAHEAD_ACCEPTS.begin() + state_nr_ * /* $replace LOOKAHEAD_ACCEPTS_CHUNK_COUNT */, /* $replace LOOKAHEAD_ACCEPTS_CHUNK_COUNT */);
}

/* $replace NAMESPACE_CLOSE */
