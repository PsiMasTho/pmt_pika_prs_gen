#pragma once

#include <cstdint>
#include <limits>
#include <string>
#include <vector>

namespace pmt::util::parse {

class GenericLexerTables {
 public:
  static inline constexpr uint64_t STATE_NR_START = 0;

  uint64_t _state_nr_sink = std::numeric_limits<uint64_t>::max();
  uint64_t _state_nr_most_frequent = std::numeric_limits<uint64_t>::max();

  uint64_t _padding_l = 0;
  uint64_t _padding_r = 0;

  std::vector<uint64_t> _transitions_default;
  std::vector<uint64_t> _transitions_shift;
  std::vector<uint64_t> _transitions_next;
  std::vector<uint64_t> _transitions_check;

  std::vector<std::vector<uint64_t>> _accepts;  // Bitsets, Indexed by state number
  std::vector<std::string> _terminal_names;     // Indexed by accept index
  std::vector<uint64_t> _terminal_ids;          // Indexed by accept index
  std::vector<std::string> _id_names;           // Indexed by id
};

}  // namespace pmt::util::parse