/* Generated on: /* $replace TIMESTAMP */ */
// clang-format off
#include "/* $replace HEADER_INCLUDE_PATH */"

#include <pmt/parser/rt/util.hpp>
#include <pmt/parser/primitives.hpp>

/* $replace NAMESPACE_OPEN */

namespace {
std::array</* $replace LEXER_TRANSITIONS_TYPE */, /* $replace LEXER_TRANSITIONS_SIZE */> const LEXER_TRANSITIONS = {
/* $replace LEXER_TRANSITIONS */
};

std::array</* $replace LEXER_TRANSITIONS_STATE_OFFSETS_TYPE */, /* $replace LEXER_TRANSITIONS_STATE_OFFSETS_SIZE */> const LEXER_TRANSITIONS_STATE_OFFSETS = {
/* $replace LEXER_TRANSITIONS_STATE_OFFSETS */
};

std::array</* $replace LEXER_TRANSITIONS_SYMBOL_KIND_OFFSETS_TYPE */, /* $replace LEXER_TRANSITIONS_SYMBOL_KIND_OFFSETS_SIZE */> const LEXER_TRANSITIONS_SYMBOL_KIND_OFFSETS = {
/* $replace LEXER_TRANSITIONS_SYMBOL_KIND_OFFSETS */
};

std::array</* $replace LINECOUNT_TRANSITIONS_TYPE */, /* $replace LINECOUNT_TRANSITIONS_SIZE */> const LINECOUNT_TRANSITIONS = {
/* $replace LINECOUNT_TRANSITIONS */
};

std::array</* $replace LINECOUNT_TRANSITIONS_STATE_OFFSETS_TYPE */, /* $replace LINECOUNT_TRANSITIONS_STATE_OFFSETS_SIZE */> const LINECOUNT_TRANSITIONS_STATE_OFFSETS = {
/* $replace LINECOUNT_TRANSITIONS_STATE_OFFSETS */
};

std::array</* $replace LINECOUNT_TRANSITIONS_SYMBOL_KIND_OFFSETS_TYPE */, /* $replace LINECOUNT_TRANSITIONS_SYMBOL_KIND_OFFSETS_SIZE */> const LINECOUNT_TRANSITIONS_SYMBOL_KIND_OFFSETS = {
/* $replace LINECOUNT_TRANSITIONS_SYMBOL_KIND_OFFSETS */
};

std::array<pmt::base::Bitset::ChunkType const, /* $replace LEXER_ACCEPTS_SIZE */> const LEXER_ACCEPTS = {
/* $replace LEXER_ACCEPTS */
};

std::array<char const* const, /* $replace LEXER_ACCEPT_LABELS_SIZE */> const LEXER_ACCEPT_LABELS = {
/* $replace LEXER_ACCEPT_LABELS */
};

std::array<pmt::parser::GenericId::IdType, /* $replace LEXER_ACCEPT_IDS_SIZE */> const LEXER_ACCEPT_IDS = {
/* $replace LEXER_ACCEPT_IDS */
};

std::array<char const* const, /* $replace ID_NAMES_SIZE */> const ID_NAMES = {
/* $replace ID_NAMES */
};

std::array</* $replace LINECOUNT_ACCEPTS_TYPE */, /* $replace LINECOUNT_ACCEPTS_SIZE */> const LINECOUNT_ACCEPTS = {
/* $replace LINECOUNT_ACCEPTS */
};

} // namespace

auto /* $replace CLASS_NAME */::get_state_nr_next(pmt::util::sm::StateNrType state_nr_, pmt::util::sm::SymbolValueType symbol_) const -> pmt::util::sm::StateNrType {
 return pmt::parser::rt::get_state_nr_next_generic(LEXER_TRANSITIONS, LEXER_TRANSITIONS_STATE_OFFSETS, LEXER_TRANSITIONS_SYMBOL_KIND_OFFSETS, state_nr_, pmt::parser::SymbolKindCharacter, symbol_);
}

auto /* $replace CLASS_NAME */::get_state_accepts(pmt::util::sm::StateNrType state_nr_) const -> pmt::base::Bitset::ChunkSpanConst {
 return pmt::base::Bitset::ChunkSpanConst(LEXER_ACCEPTS.begin() + state_nr_ * /* $replace LEXER_ACCEPT_CHUNK_COUNT */, /* $replace LEXER_ACCEPT_CHUNK_COUNT */);
}

auto /* $replace CLASS_NAME */::get_accept_count() const -> size_t {
 return /* $replace LEXER_ACCEPT_COUNT */;
}

auto /* $replace CLASS_NAME */::get_start_accept_index() const -> size_t {
 return /* $replace START_ACCEPT_INDEX */;
}

auto /* $replace CLASS_NAME */::get_eoi_accept_index() const -> size_t {
 return /* $replace EOI_ACCEPT_INDEX */;
}

auto /* $replace CLASS_NAME */::get_accept_index_label(size_t index_) const -> std::string {
 return LEXER_ACCEPT_LABELS[index_];
}

auto /* $replace CLASS_NAME */::get_accept_index_id(size_t index_) const -> pmt::parser::GenericId::IdType {
 return LEXER_ACCEPT_IDS[index_];
}

auto /* $replace CLASS_NAME */::id_to_string(pmt::parser::GenericId::IdType id_) const -> std::string {
 return ID_NAMES[id_ - /* $replace MIN_ID */];
}

auto /* $replace CLASS_NAME */::get_min_id() const -> pmt::parser::GenericId::IdType {
 return /* $replace MIN_ID */;
}

auto /* $replace CLASS_NAME */::get_id_count() const -> size_t {
 return /* $replace ID_COUNT */;
}

auto /* $replace CLASS_NAME */::get_linecount_state_nr_next(pmt::util::sm::StateNrType state_nr_, pmt::util::sm::SymbolValueType symbol_) const -> pmt::util::sm::StateNrType {
 return pmt::parser::rt::get_state_nr_next_generic(LINECOUNT_TRANSITIONS, LINECOUNT_TRANSITIONS_STATE_OFFSETS, LINECOUNT_TRANSITIONS_SYMBOL_KIND_OFFSETS, state_nr_, pmt::parser::SymbolKindCharacter, symbol_);
}

auto /* $replace CLASS_NAME */::is_linecount_state_nr_accepting(pmt::util::sm::SymbolValueType state_nr_) const -> bool {
 return std::binary_search(LINECOUNT_ACCEPTS.begin(), LINECOUNT_ACCEPTS.end(), state_nr_);
}

/* $replace NAMESPACE_CLOSE */
// clang-format on