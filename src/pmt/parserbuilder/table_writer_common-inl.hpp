// clang-format off
#ifdef __INTELLISENSE__
    #include "pmt/parserbuilder/table_writer_common.hpp"
#endif
// clang-format on

#include <vector>
#include <sstream>
#include <limits>

namespace pmt::parserbuilder {

template <std::integral T_>
void TableWriterCommon::write_single_entries(std::ostream& os_, std::span<T_ const> const& entries_) {
  std::vector<std::string> entries_str;
  size_t max_width = 0;
  for (T_ const& entry : entries_) {
   bool const is_negated = entry == std::numeric_limits<T_>::max() && std::is_unsigned_v<T_>;
     std::string entry_str = as_hex(entry, false);
     entries_str.push_back(entry_str);
     max_width = std::max(max_width, entries_str.back().size());
  }

  // Pad the entries with leading zeros to be the same width
  for (auto& entry_str : entries_str) {
    entry_str.insert(entry_str.begin(), max_width - entry_str.size(), '0');
    entry_str.insert(0, "0x");
  }

  size_t const entries_per_line = calculate_numeric_entries_per_line(max_width);

  std::string delim;
  for (size_t i = 0; i < entries_str.size(); ++i) {
    if (i == 0) {
      os_ << " ";
    } else if (i % entries_per_line == 0) {
      os_ << ",\n ";
    } else {
      os_ << ", ";
    }
    os_ << entries_str[i];
  }
}

auto TableWriterCommon::as_hex(std::integral auto value_, bool hex_prefix_) -> std::string {
 std::ostringstream os;
 if (hex_prefix_) {
   os << "0x";
 }
 os << std::hex << value_;

 return os.str();
}

}