// clang-format off
#ifdef __INTELLISENSE__
    #include "pmt/parserbuilder/table_writer_common.hpp"
#endif
// clang-format on

#include <vector>
#include <sstream>
#include <limits>
#include <unordered_set>

namespace pmt::parserbuilder {

template <std::integral T_>
void TableWriterCommon::write_single_entries(std::ostream& os_, std::span<T_ const> const& entries_, std::string const& label_) {
  std::vector<std::string> entries_str;
  std::unordered_set<size_t> negated_entries;
  size_t max_width = 0;
  for (size_t i = 0; T_ const& entry : entries_) {
   bool const is_negated = entry == std::numeric_limits<T_>::max() && std::is_unsigned_v<T_>;
    if (is_negated) {
     negated_entries.insert(i);
     entries_str.push_back("0");
    } else {
     std::string entry_str = as_hex(entry, false);
     entries_str.push_back(entry_str);
    }

    max_width = std::max(max_width, entries_str.back().size());
    ++i;
  }

  // Pad the entries with leading zeros to be the same width
  for (size_t i = 0; auto& entry_str : entries_str) {
    entry_str.insert(entry_str.begin(), max_width - entry_str.size(), '0');
        
    entry_str.insert(0, "0x");
    
    if (negated_entries.contains(i)) {
     entry_str.insert(0, "~");
   }
    
    entry_str.append("ULL");

    ++i;
  }

  size_t const entries_per_line = calculate_numeric_entries_per_line(max_width);

  os_ << label_ << " = {\n";
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
  os_ << "\n};\n";
}

auto TableWriterCommon::as_hex(std::integral auto value_, bool hex_prefix_, bool ull_postfix_) -> std::string {
 std::ostringstream os;
 if (hex_prefix_) {
   os << "0x";
 }
 os << std::hex << value_;

 if (ull_postfix_) {
   os << "ULL";
 }
 
 return os.str();
}

}