#pragma once

#include "pmt/base/hashable.hpp"
#include "pmt/util/smrt/state_machine_primitives.hpp"

#include <climits>

namespace pmt::util::smct {

class Symbol : public pmt::base::Hashable<Symbol> {
  // -$ Data $-
  pmt::util::smrt::SymbolValueType _value;
  pmt::util::smrt::SymbolKindType _kind;

 public:
  // -$ Functions $-
  // --$ Lifetime $--
  Symbol() = default;
  Symbol(pmt::util::smrt::SymbolKindType kind_, pmt::util::smrt::SymbolValueType value_);

  // --$ Operators $--
  auto operator==(Symbol const& other_) const -> bool = default;
  auto operator!=(Symbol const& other_) const -> bool = default;

  // --$ Inherited: pmt::base::Hashable $--
  auto hash() const -> size_t;

  // --$ Other $--
  auto get_kind() const -> pmt::util::smrt::SymbolKindType;
  auto get_value() const -> pmt::util::smrt::SymbolValueType;

  void set_kind(pmt::util::smrt::SymbolKindType kind_);
  void set_value(pmt::util::smrt::SymbolValueType value_);
};

}  // namespace pmt::util::smct