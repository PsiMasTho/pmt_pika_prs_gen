// clang-format off
#ifdef __INTELLISENSE__
    #include "pmt/parser/builder/table_writer_common.hpp"
#endif
// clang-format on

#include <algorithm>
#include <vector>
#include <sstream>
#include <limits>

namespace pmt::parser::builder {

template <typename T_>
requires std::integral<typename T_::value_type>
void TableWriterCommon::replace_array(pmt::util::SkeletonReplacerBase& skeleton_replacer_, std::string& str_, std::string const& label_, T_ begin_, T_ end_) {
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

  std::string replacement_str;
  std::string delim;
  for (size_t i = 0; i < entries_str.size(); ++i) {
    if (i == 0) {
      replacement_str += " ";
    } else if (i % entries_per_line == 0) {
      replacement_str += ",\n ";
    } else {
      replacement_str += ", ";
    }
    replacement_str += entries_str[i];
  }

  skeleton_replacer_.replace_skeleton_label(str_, label_, replacement_str);
  skeleton_replacer_.replace_skeleton_label(str_, label_ + "_TYPE", TableWriterCommon::get_smallest_unsigned_type<T_>(begin_, end_) + " const");
  skeleton_replacer_.replace_skeleton_label(str_, label_ + "_SIZE", TableWriterCommon::as_hex(std::distance(begin_, end_), true));
}

template <std::integral T_>
void TableWriterCommon::replace_array(pmt::util::SkeletonReplacerBase& skeleton_replacer_, std::string& str_, std::string const& label_, std::span<T_ const> const& entries_) {
 replace_array(skeleton_replacer_, str_, label_, entries_.begin(), entries_.end());
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
  size_t const max = (begin_ == end_) ? 0 : *std::max_element(begin_, end_);
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