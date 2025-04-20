#pragma once

#include "pmt/base/hashable.hpp"

#include <climits>
#include <cstdint>

namespace pmt::util::smct {

class Symbol : public pmt::base::Hashable<Symbol> {
 public:
  // -$ Types / Constants $-
  using UnderlyingType = uint64_t;

  enum : size_t {
    UnderlyingTypeBitWidth = sizeof(UnderlyingType) * CHAR_BIT,
    KindBitWidth = 8ull,
    ValueBitWidth = UnderlyingTypeBitWidth - KindBitWidth,
    KindMax = (1ull << KindBitWidth) - 1ull,
    ValueMax = (1ull << ValueBitWidth) - 1ull,
  };

 private:
  // -$ Data $-
  UnderlyingType _kind : KindBitWidth;
  UnderlyingType _value : ValueBitWidth;

 public:
  // -$ Functions $-
  // --$ Lifetime $--
  Symbol() = default;
  explicit Symbol(UnderlyingType combined_);
  Symbol(UnderlyingType kind_, UnderlyingType value_);

  // --$ Operators $--
  auto operator==(Symbol const& other_) const -> bool = default;
  auto operator!=(Symbol const& other_) const -> bool = default;

  // --$ Inherited: pmt::base::Hashable $--
  auto hash() const -> size_t;

  // --$ Other $--
  auto get_kind() const -> UnderlyingType;
  auto get_value() const -> UnderlyingType;
  auto get_combined() const -> UnderlyingType;

  void set_kind(UnderlyingType kind_);
  void set_value(UnderlyingType value_);
};

}  // namespace pmt::util::smct