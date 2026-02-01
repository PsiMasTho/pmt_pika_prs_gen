#include "pmt/util/read_file.hpp"

#include <cstdio>
#include <memory>

namespace pmt::util {

namespace {

using FileDescriptor = std::unique_ptr<std::FILE, decltype([](std::FILE* fd_) { std::fclose(fd_); })>;

enum : size_t {
 ChunkSize = 1024 * 8,
};

}  // namespace

auto read_file(std::string_view file_path_) -> std::string {
 FileDescriptor fd{std::fopen(file_path_.data(), "rb")};

 if (!fd) {
  throw std::runtime_error("Failed to open file: " + std::string{file_path_});
 }

 std::string buffer(ChunkSize, '\0');

 size_t total_bytes_read = 0;

 while (size_t const bytes_read = std::fread(buffer.data() + total_bytes_read, 1, ChunkSize, fd.get())) {
  total_bytes_read += bytes_read;

  if (std::ferror(fd.get())) {
   throw std::runtime_error("Failed to read file: " + std::string{file_path_});
  }

  if (std::feof(fd.get())) {
   break;
  }

  buffer.resize(total_bytes_read + ChunkSize);
 }

 buffer.resize(total_bytes_read);
 return buffer;
}

}  // namespace pmt::util