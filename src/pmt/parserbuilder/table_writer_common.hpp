#pragma once

#include <ostream>
#include <string>
#include <span>
#include <concepts>

namespace pmt::parserbuilder {

class TableWriterCommon {
 public:
 template <std::integral T_>
 static void write_single_entries(std::ostream& os_, std::span<T_ const> const& entries_);

 static void write_single_entries(std::ostream& os_, std::span<std::string const> const& entries_);

 static auto as_hex(std::integral auto value_, bool hex_prefix_ = true) -> std::string;

 template <std::integral T_>
 static auto get_smallest_unsigned_type(std::span<T_ const> data_) -> std::string;

 private:
 static auto calculate_numeric_entries_per_line(size_t max_width_) -> size_t;
};

}

#include "pmt/parserbuilder/table_writer_common-inl.hpp"