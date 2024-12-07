#pragma once

#include "pmt/base/dynamic_bitset.hpp"
#include "pmt/util/parse/fa.hpp"
#include "pmt/util/parse/terminal_info.hpp"

#include <array>
#include <limits>
#include <string>
#include <vector>

namespace pmt::util::parse {

class GenericLexerTables {
 public:
  static inline constexpr Fa::StateNrType INVALID_STATE_NR = std::numeric_limits<Fa::StateNrType>::max();

  Fa::StateNrType _state_nr_start;
  std::vector<std::array<Fa::StateNrType, UCHAR_MAX>> _transitions;  // Indexed by state number
  std::vector<pmt::base::DynamicBitset> _accepts;                    // Indexed by state number
  std::vector<TerminalInfo> _terminals;                              // Indexed by accept index
  std::vector<std::string> _id_names;                                // Indexed by id
};

}  // namespace pmt::util::parse