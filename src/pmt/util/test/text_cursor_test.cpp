#include "pmt/util/test/text_cursor_test.hpp"

#include "pmt/util/text_cursor.hpp"
#include "pmt/util/text_encoding.hpp"

#include <chrono>
#include <fstream>
#include <iostream>
#include <iterator>

namespace pmt::util::test {

namespace {
auto read_file_into_string(std::string const& path_) -> std::string {
  std::ifstream file{path_, std::ios::binary};

  if (!file.is_open())
    return {};

  return {std::istreambuf_iterator<char>{file}, std::istreambuf_iterator<char>{}};
}
}  // namespace

void TextCursorTest::run(int argc_, char const* const* argv_) {
  if (argc_ != 2) {
    std::cerr << "Usage: " << argv_[0] << " <file>" << std::endl;
    return;
  }

  auto file_contents = read_file_into_string(argv_[1]);
  if (file_contents.empty()) {
    std::cerr << "Failed to read file" << std::endl;
    return;
  }

  TextCursor cursor(file_contents, ENCODING_UTF8);

  auto const start = std::chrono::high_resolution_clock::now();
  size_t count = 0;
  while (!cursor.is_at_end()) {
    CodepointType const cp = cursor.read();

    if (cp == INVALID_CODEPOINT) {
      std::cerr << "Invalid codepoint" << std::endl;
      return;
    } else {
      std::cout << (uint32_t)cp << std::endl;
    }

    ++count;
  }
  auto const end = std::chrono::high_resolution_clock::now();

  std::chrono::milliseconds duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

  std::cout << "Read " << count << " codepoints in " << duration << std::endl;
}

}  // namespace pmt::util::test