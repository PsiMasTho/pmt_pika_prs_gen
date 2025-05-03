#include "pmt/util/smrt/lexer_tables_base.hpp"

namespace pmt::util::smrt {
using namespace pmt::base;

auto LexerTablesBase::get_accept_string(size_t index_) const -> std::string {
  return std::to_string(index_);
}

auto LexerTablesBase::get_accept_id(size_t) const -> GenericId::IdType {
  return GenericId::IdUninitialized;
}

auto LexerTablesBase::id_to_string(GenericId::IdType id_) const -> std::string {
  return std::to_string(id_);
}

auto LexerTablesBase::get_state_nr_next_impl(pmt::util::smrt::StateNrType state_nr_, pmt::util::smrt::SymbolType symbol_, std::span<pmt::util::smrt::SymbolType const> lowers_, std::span<pmt::util::smrt::SymbolType const> uppers_, std::span<pmt::util::smrt::StateNrType const> values_, std::span<size_t const> offsets_) -> pmt::util::smrt::StateNrType {
 size_t const offset = offsets_[state_nr_];
 size_t const offset_next = offsets_[state_nr_ + 1];
 std::span<pmt::util::smrt::SymbolType const> const lowers = lowers_.subspan(offset, offset_next - offset);
 std::span<pmt::util::smrt::SymbolType const> const uppers = uppers_.subspan(offset, offset_next - offset);

 size_t const idx = [&]() {
  auto const itr = std::lower_bound(lowers.begin(), lowers.end(), symbol_);
  return std::distance(lowers.begin(), itr);
 }();

if (idx == lowers.size()) {
  if (idx == 0 || symbol_ > uppers[idx - 1]) {
   return pmt::util::smrt::StateNrSink;
  }
  
  return values_[offset + idx - 1];
}

if (symbol_ < lowers[idx]) {
  if (idx == 0) {
    return pmt::util::smrt::StateNrSink;
  }
  if (symbol_ <= uppers[idx - 1]) {
    return values_[offset + idx - 1];
  }
  return pmt::util::smrt::StateNrSink;
 }

 return values_[offset + idx];
}

}  // namespace pmt::util::smrt