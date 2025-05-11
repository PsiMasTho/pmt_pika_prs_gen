/* Generated on: /* $replace TIMESTAMP */ */
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

pmt::base::Bitset::ChunkType const TERMINALS[] = {
/* $replace TERMINALS */
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

/* $replace NEWLINE_LOWER_BOUNDS_TYPE */ const NEWLINE_LOWER_BOUNDS[] = {
/* $replace NEWLINE_LOWER_BOUNDS */
};

/* $replace NEWLINE_UPPER_BOUNDS_TYPE */ const NEWLINE_UPPER_BOUNDS[] = {
/* $replace NEWLINE_UPPER_BOUNDS */
};

/* $replace NEWLINE_VALUES_TYPE */ const NEWLINE_VALUES[] = {
/* $replace NEWLINE_VALUES */
};

/* $replace NEWLINE_OFFSETS_TYPE */ const NEWLINE_OFFSETS[] = {
/* $replace NEWLINE_OFFSETS */
};

auto encode_symbol(pmt::util::smrt::SymbolType symbol_) -> pmt::util::smrt::SymbolType {
 return (symbol_ == pmt::util::smrt::SymbolValueMax) ? 0 : symbol_ + 1;
}

auto decode_symbol(pmt::util::smrt::SymbolType symbol_) -> pmt::util::smrt::SymbolType {
 return (symbol_ == 0) ? pmt::util::smrt::SymbolValueMax : symbol_ - 1;
}

auto lowers_lt(pmt::util::smrt::SymbolType lhs_, pmt::util::smrt::SymbolType rhs_) -> bool {
 return decode_symbol(lhs_) < decode_symbol(rhs_);
}

template <typename LOWER_T_, typename UPPER_T_, typename VALUE_T_, typename OFFSET_T_>
auto get_state_nr_next_generic( std::span<LOWER_T_ const> lowers_, std::span<UPPER_T_ const> uppers_, std::span<VALUE_T_ const> values_, std::span<OFFSET_T_ const> offsets_, pmt::util::smrt::StateNrType state_nr_, pmt::util::smrt::SymbolType symbol_) -> pmt::util::smrt::StateNrType {
 std::span<LOWER_T_ const> lowers_span(&lowers_[offsets_[state_nr_]], offsets_[state_nr_ + 1] - offsets_[state_nr_]);
 std::span<UPPER_T_ const> uppers_span(&uppers_[offsets_[state_nr_]], offsets_[state_nr_ + 1] - offsets_[state_nr_]);

 size_t const idx = std::distance(lowers_span.begin(), std::lower_bound(lowers_span.begin(), lowers_span.end(), encode_symbol(symbol_), lowers_lt));

 if (idx == lowers_span.size()) {
  if (idx == 0 || symbol_ > decode_symbol(uppers_span[idx - 1])) {
   return pmt::util::smrt::StateNrSink;
  }
  return values_[offsets_[state_nr_] + idx - 1];
 }

 if (symbol_ < decode_symbol(lowers_span[idx])) {
  if (idx == 0) {
   return pmt::util::smrt::StateNrSink;
  }
  if (symbol_ <= decode_symbol(uppers_span[idx - 1])) {
   return values_[offsets_[state_nr_] + idx - 1];
  }
  return pmt::util::smrt::StateNrSink;
 }

 return values_[offsets_[state_nr_] + idx];
}

} // namespace

auto /* $replace CLASS_NAME */::get_state_nr_next(pmt::util::smrt::StateNrType state_nr_, pmt::util::smrt::SymbolType symbol_) const -> pmt::util::smrt::StateNrType {
 return get_state_nr_next_generic</* $replace LOWER_BOUNDS_TYPE */, /* $replace UPPER_BOUNDS_TYPE */, /* $replace VALUES_TYPE */, /* $replace OFFSETS_TYPE */>(LOWER_BOUNDS, UPPER_BOUNDS, VALUES, OFFSETS, state_nr_, symbol_);
}

auto /* $replace CLASS_NAME */::get_state_terminals(pmt::util::smrt::StateNrType state_nr_) const -> pmt::base::Bitset::ChunkSpanConst {
 return pmt::base::Bitset::ChunkSpanConst(TERMINALS + state_nr_ * /* $replace TERMINAL_COUNT */, /* $replace TERMINAL_COUNT */);
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

auto /* $replace CLASS_NAME */::get_newline_state_nr_next(pmt::util::smrt::StateNrType state_nr_, pmt::util::smrt::SymbolType symbol_) const -> pmt::util::smrt::StateNrType {
 return get_state_nr_next_generic</* $replace NEWLINE_LOWER_BOUNDS_TYPE */, /* $replace NEWLINE_UPPER_BOUNDS_TYPE */, /* $replace NEWLINE_VALUES_TYPE */, /* $replace NEWLINE_OFFSETS_TYPE */>(NEWLINE_LOWER_BOUNDS, NEWLINE_UPPER_BOUNDS, NEWLINE_VALUES, NEWLINE_OFFSETS, state_nr_, symbol_);
}

auto /* $replace CLASS_NAME */::get_newline_state_nr_accept() const -> pmt::util::smrt::SymbolType {
 return /* $replace NEWLINE_ACCEPTING_STATE_NR */;
}

/* $replace NAMESPACE_CLOSE */
