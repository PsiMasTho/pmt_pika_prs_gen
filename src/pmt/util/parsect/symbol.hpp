#pragma once

#include "pmt/base/hashable.hpp"

#include <cstdint>

namespace pmt::util::parsect {

class Symbol : public pmt::base::Hashable<Symbol> {
 public:
  // -$ Types / Constants $-
  using KindType = uint8_t;
  using ValueType = uint32_t;

  // -$ Data $-
  KindType _kind;
  ValueType _value;

  // -$ Functions $-
  // --$ Lifetime $--
  Symbol() = default;
  Symbol(KindType kind_, ValueType value_);

  // --$ Inherited: pmt::base::Hashable $--
  auto hash() const -> size_t;
};

}  // namespace pmt::util::parsect