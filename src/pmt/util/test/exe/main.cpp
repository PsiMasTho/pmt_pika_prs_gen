#include <chrono>
#include <fstream>
#include <iostream>
#include <iterator>

#include "pmt/util/text_encoding.hpp"
#include "pmt/util/text_stream.hpp"

auto read_file_into_string(std::string const& path_) -> std::u8string {
  std::ifstream file{path_, std::ios::binary};

  if (!file.is_open())
    return {};

  return {std::istreambuf_iterator<char>{file}, std::istreambuf_iterator<char>{}};
}

auto main(int argc, char const* const* argv) -> int {
  if (argc != 2) {
    std::cerr << "Usage: " << argv[0] << " <file>" << std::endl;
    return 1;
  }

  auto file_contents = read_file_into_string(argv[1]);
  if (file_contents.empty()) {
    std::cerr << "Failed to read file" << std::endl;
    return 1;
  }

  auto text_stream = pmt::util::text_stream::construct<pmt::util::ENCODING_UTF8>(file_contents);

  auto const start = std::chrono::high_resolution_clock::now();
  size_t count = 0;
  while (!text_stream.is_at_end()) {
    pmt::util::codepoint_type const cp = text_stream.read();

    if (cp == pmt::util::INVALID_CODEPOINT) {
      std::cerr << "Invalid codepoint" << std::endl;
      return 1;
    } else {
      std::cout << (uint32_t)cp << std::endl;
    }

    ++count;
  }
  auto const end = std::chrono::high_resolution_clock::now();

  std::chrono::milliseconds duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

  std::cout << "Read " << count << " codepoints in " << duration << std::endl;

  return 0;
}