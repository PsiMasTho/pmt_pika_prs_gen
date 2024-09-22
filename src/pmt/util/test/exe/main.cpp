#include <chrono>
#include <fstream>
#include <iostream>
#include <iterator>
#include <string_view>

#include "pmt/util/text_encoding.hpp"
#include "pmt/util/text_view.hpp"

auto read_file_into_string(std::string path_) -> std::string {
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

  pmt::util::text_view text_view{std::string_view{file_contents}, pmt::util::UTF8};

  auto const start = std::chrono::high_resolution_clock::now();
  size_t count = 0;
  while (!text_view.is_at_end()) {
    pmt::util::text_view::codepoint_type const cp = text_view.read();

    if (cp == pmt::util::text_view::INVALID_CODEPOINT) {
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