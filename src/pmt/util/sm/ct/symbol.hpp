#pragma once

#include "pmt/base/hashable.hpp"
#include "pmt/util/sm/primitives.hpp"

#include <climits>

namespace pmt::util::sm::ct {

class Symbol : public pmt::base::Hashable<Symbol> {
  // -$ Data $-
  SymbolValueType _value;
  SymbolKindType _kind;

 public:
  // -$ Functions $-
  // --$ Lifetime $--
  Symbol() = default;
  Symbol(SymbolKindType kind_, SymbolValueType value_);

  // --$ Operators $--
  auto operator==(Symbol const& other_) const -> bool = default;
  auto operator!=(Symbol const& other_) const -> bool = default;

  // --$ Inherited: pmt::base::Hashable $--
  auto hash() const -> size_t;

  // --$ Other $--
  auto get_kind() const -> SymbolKindType;
  auto get_value() const -> SymbolValueType;

  void set_kind(SymbolKindType kind_);
  void set_value(SymbolValueType value_);
};

}  // namespace pmt::util::smct