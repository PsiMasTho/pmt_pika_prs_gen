#pragma once

#include "pmt/base/hashable.hpp"
#include "pmt/util/smrt/state_machine_primitives.hpp"

#include <climits>

namespace pmt::util::smct {

class Symbol : public pmt::base::Hashable<Symbol> {
 public:
  // -$ Types / Constants $-
  enum : size_t {
    KindBitWidth = 8ull,
    ValueBitWidth = sizeof(pmt::util::smrt::SymbolType) * CHAR_BIT - KindBitWidth,
    KindMax = (1ull << KindBitWidth) - 1ull,
    ValueMax = (1ull << ValueBitWidth) - 1ull,
  };

 private:
  // -$ Data $-
  pmt::util::smrt::SymbolType _kind : KindBitWidth;
  pmt::util::smrt::SymbolType _value : ValueBitWidth;

 public:
  // -$ Functions $-
  // --$ Lifetime $--
  Symbol() = default;
  explicit Symbol(pmt::util::smrt::SymbolType combined_);
  Symbol(pmt::util::smrt::SymbolType kind_, pmt::util::smrt::SymbolType value_);

  // --$ Operators $--
  auto operator==(Symbol const& other_) const -> bool = default;
  auto operator!=(Symbol const& other_) const -> bool = default;

  // --$ Inherited: pmt::base::Hashable $--
  auto hash() const -> size_t;

  // --$ Other $--
  auto get_kind() const -> pmt::util::smrt::SymbolType;
  auto get_value() const -> pmt::util::smrt::SymbolType;
  auto get_combined() const -> pmt::util::smrt::SymbolType;

  void set_kind(pmt::util::smrt::SymbolType kind_);
  void set_value(pmt::util::smrt::SymbolType value_);
};

}  // namespace pmt::util::smct