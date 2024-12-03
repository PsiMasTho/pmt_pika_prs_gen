#pragma once

#include "pmt/base/dynamic_bitset.hpp"
#include "pmt/util/parse/fa.hpp"
#include "pmt/util/parse/terminal_info.hpp"

#include <array>
#include <limits>
#include <vector>

namespace pmt::util::parse {

class GenericLexerTables {
 public:
  static inline constexpr Fa::StateNrType INVALID_STATE_NR = std::numeric_limits<Fa::StateNrType>::max();

  Fa::StateNrType _state_nr_start;
  std::vector<std::array<Fa::StateNrType, 256>> _transitions;
  std::vector<pmt::base::DynamicBitset> _accepts;
  std::vector<TerminalInfo> _terminals;
};

}  // namespace pmt::util::parse