#include "pmt/util/text_encoding.hpp"

#include "pmt/util/endianness.hpp"

namespace pmt::util {

const text_encoding UTF8 = {._endian = endianness::NATIVE, ._byte_count = 1};
const text_encoding UTF16 = {._endian = endianness::NATIVE, ._byte_count = 2};
const text_encoding UTF32 = {._endian = endianness::NATIVE, ._byte_count = 4};

const text_encoding UTF16BE = {._endian = endianness::BIG, ._byte_count = 2};
const text_encoding UTF32BE = {._endian = endianness::BIG, ._byte_count = 4};

const text_encoding UTF16LE = {._endian = endianness::LITTLE, ._byte_count = 2};
const text_encoding UTF32LE = {._endian = endianness::LITTLE, ._byte_count = 4};

}  // namespace pmt::util