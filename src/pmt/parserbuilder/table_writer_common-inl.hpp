// clang-format off
#ifdef __INTELLISENSE__
    #include "pmt/parserbuilder/table_writer_common.hpp"
#endif
// clang-format on

#include <algorithm>
#include <vector>
#include <sstream>
#include <limits>

namespace pmt::parserbuilder {

template <typename T_>
requires std::integral<typename T_::value_type>
void TableWriterCommon::write_single_entries(std::ostream& os_, T_ begin_, T_ end_) {
  std::vector<std::string> entries_str;
  size_t max_width = 0;
  for (auto itr = begin_; itr != end_; ++itr) {
     std::string entry_str = as_hex(*itr, false);
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

template <std::integral T_>
void TableWriterCommon::write_single_entries(std::ostream& os_, std::span<T_ const> const& entries_) {
 write_single_entries(os_, entries_.begin(), entries_.end());
}

auto TableWriterCommon::as_hex(std::integral auto value_, bool hex_prefix_) -> std::string {
 std::ostringstream os;
 if (hex_prefix_) {
   os << "0x";
 }
 os << std::hex << value_;

 return os.str();
}

template <typename T_>
requires std::integral<typename T_::value_type>
auto TableWriterCommon::get_smallest_unsigned_type(T_ begin_, T_ end_) -> std::string {
  size_t const max = *std::max_element(begin_, end_);
  if (max <= std::numeric_limits<uint8_t>::max()) {
    return "uint8_t";
  } else if (max <= std::numeric_limits<uint16_t>::max()) {
    return "uint16_t";
  } else if (max <= std::numeric_limits<uint32_t>::max()) {
    return "uint32_t";
  } else {
    return "uint64_t";
  }
}

template <std::integral T_>
auto TableWriterCommon::get_smallest_unsigned_type(std::span<T_ const> data_) -> std::string {
 return get_smallest_unsigned_type(data_.begin(), data_.end());
}

}