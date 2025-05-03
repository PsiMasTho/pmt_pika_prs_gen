#include "pmt/parserbuilder/table_writer_common.hpp"

#include "pmt/asserts.hpp"

namespace pmt::parserbuilder {

 void TableWriterCommon::write_single_entries(std::ostream& os_, std::span<std::string const> const& entries_, std::string const& label_) {
  os_ << label_ << " = {\n";
  std::string delim;
  for (size_t i = 0; i < entries_.size(); ++i) {
    if (i == 0) {
      os_ << " ";
    } else {
      os_ << ",\n ";
    }
    os_ << '"' << entries_[i] << '"';
  }
  os_ << "\n};\n";
}

auto TableWriterCommon::calculate_numeric_entries_per_line(size_t max_width_) -> size_t {
 switch (max_width_) {
  case 1:
   return 25;
  case 2:
   return 22;
  case 3:
   return 20;
  case 4:
   return 18;
  case 5:
   return 17;
  case 6:
   return 15;
  case 7:
   return 14;
  case 8:
   return 13;
  case 9:
   return 12;
  case 10:
   return 11;
  case 11:
   return 11;
  case 12:
   return 10;
  case 13:
   return 10;
  case 14:
   return 9;
  case 15:
   return 9;
  case 16:
   return 8;
  default:
   pmt::unreachable();
 }
}

}