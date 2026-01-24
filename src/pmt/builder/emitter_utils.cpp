#include "pmt/builder/emitter_utils.hpp"

#include <istream>
#include <stdexcept>

namespace pmt::builder {

auto read_stream(std::istream& file_, std::string_view label_) -> std::string {
 if (!file_.good()) {
  throw std::runtime_error("Failed to open file: " + std::string(label_));
 }
 file_.clear();
 file_.seekg(0, std::ios::beg);
 return std::string(std::istreambuf_iterator<char>(file_), std::istreambuf_iterator<char>());
}

}  // namespace pmt::builder
