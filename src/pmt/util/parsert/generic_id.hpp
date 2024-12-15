#pragma once

#include <cstdint>
#include <string>

namespace pmt::util::parsert {

class GenericId {
 public:
  using IdType = uint64_t;

  enum : IdType {
    IdUninitialized = -1ULL,  // Underflow
    IdDefault = -2ULL,
    IdEoi = -3ULL,
  };

  static auto id_to_string(IdType id_) -> std::string;
  static auto string_to_id(std::string_view str_) -> IdType;

  static auto is_generic_id(IdType id_) -> bool;
};

}  // namespace pmt::util::parsert