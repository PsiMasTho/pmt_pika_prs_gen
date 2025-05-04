/* $replace TIMESTAMP */
#include "/* $replace HEADER_INCLUDE_PATH */"

/* $replace NAMESPACE_OPEN */

namespace {
/* $replace LOWER_BOUNDS_TYPE */ const LOWER_BOUNDS[] = {
/* $replace LOWER_BOUNDS */
};

/* $replace UPPER_BOUNDS_TYPE */ const UPPER_BOUNDS[] = {
/* $replace UPPER_BOUNDS */
};

/* $replace VALUES_TYPE */ const VALUES[] = {
/* $replace VALUES */
};

/* $replace OFFSETS_TYPE */ const OFFSETS[] = {
/* $replace OFFSETS */
};

pmt::base::Bitset::ChunkType const ACCEPTS[] = {
/* $replace ACCEPTS */
};

char const* const TERMINAL_LABELS[] = {
/* $replace TERMINAL_LABELS */
};

pmt::util::smrt::GenericId::IdType const TERMINAL_IDS[] = {
/* $replace TERMINAL_IDS */
};

char const* const ID_NAMES[] = {
/* $replace ID_NAMES */
};

auto lowers_lt(pmt::util::smrt::SymbolType lhs_, pmt::util::smrt::SymbolType rhs_) -> bool {
 return lhs_ - 1 < rhs_ - 1;
}

} // namespace

auto /* $replace CLASS_NAME */::get_state_nr_next(pmt::util::smrt::StateNrType state_nr_, pmt::util::smrt::SymbolType symbol_) const -> pmt::util::smrt::StateNrType {
 std::span</* $replace LOWER_BOUNDS_TYPE */ const> const lowers(&LOWER_BOUNDS[OFFSETS[state_nr_]], OFFSETS[state_nr_ + 1] - OFFSETS[state_nr_]);
 std::span</* $replace UPPER_BOUNDS_TYPE */ const> const uppers(&UPPER_BOUNDS[OFFSETS[state_nr_]], OFFSETS[state_nr_ + 1] - OFFSETS[state_nr_]);

 size_t const idx = std::distance(lowers.begin(), std::lower_bound(lowers.begin(), lowers.end(), symbol_ + 1, lowers_lt));

 if (idx == lowers.size()) {
  if (idx == 0 || symbol_ > pmt::util::smrt::SymbolType(uppers[idx - 1]) - 1) {
   return pmt::util::smrt::StateNrSink;
  }
  
  return VALUES[OFFSETS[state_nr_] + idx - 1];
 }

 if (symbol_ < pmt::util::smrt::SymbolType(lowers[idx]) - 1) {
  if (idx == 0) {
    return pmt::util::smrt::StateNrSink;
  }
  if (symbol_ <= pmt::util::smrt::SymbolType(uppers[idx - 1]) - 1) {
    return VALUES[OFFSETS[state_nr_] + idx - 1];
  }
  return pmt::util::smrt::StateNrSink;
 }

 return VALUES[OFFSETS[state_nr_] + idx];
}

auto /* $replace CLASS_NAME */::get_state_terminals(pmt::util::smrt::StateNrType state_nr_) const -> pmt::base::Bitset::ChunkSpanConst {
 return pmt::base::Bitset::ChunkSpanConst(ACCEPTS + state_nr_ * /* $replace TERMINAL_COUNT */, /* $replace TERMINAL_COUNT */);
}

auto /* $replace CLASS_NAME */::get_terminal_count() const -> size_t {
 return /* $replace TERMINAL_COUNT */;
}

auto /* $replace CLASS_NAME */::get_start_terminal_index() const -> size_t {
 return /* $replace START_TERMINAL_INDEX */;
}

auto /* $replace CLASS_NAME */::get_eoi_terminal_index() const -> size_t {
 return /* $replace EOI_TERMINAL_INDEX */;
}

auto /* $replace CLASS_NAME */::get_terminal_label(size_t index_) const -> std::string {
 return TERMINAL_LABELS[index_];
}

auto /* $replace CLASS_NAME */::get_terminal_id(size_t index_) const -> pmt::util::smrt::GenericId::IdType {
 return TERMINAL_IDS[index_];
}

auto /* $replace CLASS_NAME */::id_to_string(pmt::util::smrt::GenericId::IdType id_) const -> std::string {
 return ID_NAMES[id_];
}

/* $replace NAMESPACE_CLOSE */
