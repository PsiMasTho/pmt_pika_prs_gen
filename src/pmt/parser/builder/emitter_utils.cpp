#include "pmt/parser/builder/emitter_utils.hpp"

#include <cctype>
#include <iomanip>
#include <sstream>
#include <stdexcept>

namespace pmt::parser::builder {

auto read_stream(std::ifstream& file_, std::string_view label_) -> std::string {
 if (!file_.is_open()) {
  throw std::runtime_error("Failed to open file: " + std::string(label_));
 }
 file_.clear();
 file_.seekg(0, std::ios::beg);
 return std::string(std::istreambuf_iterator<char>(file_), std::istreambuf_iterator<char>());
}

auto format_hex(uint64_t value_, size_t width_) -> std::string {
 std::ostringstream os;
 os << "0x" << std::hex << std::nouppercase << std::setw(static_cast<int>(width_)) << std::setfill('0') << value_;
 return os.str();
}

}  // namespace pmt::parser::builder
