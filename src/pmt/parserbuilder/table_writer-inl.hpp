// clang-format off
#ifdef __INTELLISENSE__
    #include "pmt/parserbuilder/table_writer.hpp"
#endif
// clang-format on

namespace pmt::parserbuilder {

template <typename T_>
void TableWriter::write_pair_entries(std::vector<T_> const& entries_, std::string const& label_) {
  std::vector<std::pair<std::string, std::string>> entries_str;
  size_t max_width = 0;
  for (auto const& [lhs, rhs] : entries_) {
    std::string lhs_str = as_hex(lhs, false);
    std::string rhs_str = as_hex(rhs, false);
    entries_str.emplace_back(lhs_str, rhs_str);
    max_width = std::max(max_width, std::max(lhs_str.size(), rhs_str.size()));
  }

  // Pad the entries with leading zeros to be the same width
  for (auto& [lhs_str, rhs_str] : entries_str) {
    lhs_str.insert(lhs_str.begin(), max_width - lhs_str.size(), '0');
    rhs_str.insert(rhs_str.begin(), max_width - rhs_str.size(), '0');
    lhs_str.insert(0, "0x");
    rhs_str.insert(0, "0x");
  }

  _os_source << label_ << " = {\n";
  std::string delim;
  for (size_t i = 0; i < entries_str.size(); ++i) {
    if (i == 0) {
      _os_source << " ";
    } else if (i % PER_LINE_HEX_PAIRS == 0) {
      _os_source << ",\n ";
    } else {
      _os_source << ", ";
    }
    _os_source << "{" << entries_str[i].first << ", " << entries_str[i].second << "}";
  }
  _os_source << "\n};\n";
}

template <typename T_>
void TableWriter::write_single_entries(std::vector<T_> const& entries_, std::string const& label_) {
  std::vector<std::string> entries_str;
  size_t max_width = 0;
  for (auto const& entry : entries_) {
    std::string entry_str = as_hex(entry, false);
    entries_str.push_back(entry_str);
    max_width = std::max(max_width, entry_str.size());
  }

  // Pad the entries with leading zeros to be the same width
  for (auto& entry_str : entries_str) {
    entry_str.insert(entry_str.begin(), max_width - entry_str.size(), '0');
    entry_str.insert(0, "0x");
  }

  _os_source << label_ << " = {\n";
  std::string delim;
  for (size_t i = 0; i < entries_str.size(); ++i) {
    if (i == 0) {
      _os_source << " ";
    } else if (i % PER_LINE == 0) {
      _os_source << ",\n ";
    } else {
      _os_source << ", ";
    }
    _os_source << entries_str[i];
  }
  _os_source << "\n};\n";
}

}  // namespace pmt::parserbuilder