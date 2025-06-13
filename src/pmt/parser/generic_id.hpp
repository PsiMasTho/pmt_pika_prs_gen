#pragma once

#include <cstdint>
#include <string>

namespace pmt::parser {

class GenericId {
 public:
  using IdType = uint64_t;

  enum : IdType {
    IdUninitialized = -1ULL,  // Underflow
    IdDefault = -2ULL,
    IdEoi = -3ULL,
    IdStart = -4ULL,
    IdNewline = -5ULL,
    IdRoot = -6ULL,
    IdReserved4 = -7ULL,
    IdReserved5 = -8ULL,
    IdReserved6 = -9ULL,
    IdReserved7 = -10ULL,
    IdReserved8 = -11ULL,
    IdReserved9 = -12ULL,
  };

  static auto id_to_string(IdType id_) -> std::string;
  static auto string_to_id(std::string_view str_) -> IdType;

  static auto is_generic_id(IdType id_) -> bool;
  static auto is_generic_id(std::string_view str_) -> bool;
};

}  // namespace pmt::parser