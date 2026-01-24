#include "pmt/builder/emitter_utils.hpp"

#include <istream>
#include <stdexcept>

namespace pmt::builder {

auto read_stream(std::istream& stream_, std::string_view label_) -> std::string {
 if (!stream_.good()) {
  throw std::runtime_error("Failed to read stream: " + std::string(label_));
 }

 return std::string(std::istreambuf_iterator<char>(stream_), std::istreambuf_iterator<char>());
}

}  // namespace pmt::builder
