#pragma once

#include "pmt/base/dynamic_bitset.hpp"
#include "pmt/util/parse/fa.hpp"
#include "pmt/util/parse/generic_ast.hpp"

#include <array>
#include <limits>
#include <string>
#include <vector>

namespace pmt::util::parse {

class GenericLexerTables {
 public:
  using TransitionType = std::array<pmt::util::parse::Fa::StateNrType, UCHAR_MAX>;
  using AcceptType = pmt::base::DynamicBitset;

  static inline constexpr Fa::StateNrType STATE_NR_START = 0;
  static inline constexpr Fa::StateNrType STATE_NR_INVALID = std::numeric_limits<Fa::StateNrType>::max();

  std::vector<TransitionType> _transitions;       // Indexed by state number
  std::vector<AcceptType> _accepts;               // Indexed by state number
  std::vector<std::string> _terminal_names;       // Indexed by accept index
  std::vector<GenericAst::IdType> _terminal_ids;  // Indexed by accept index
  std::vector<std::string> _id_names;             // Indexed by id
};

}  // namespace pmt::util::parse