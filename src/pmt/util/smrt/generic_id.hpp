#pragma once

#include <cstdint>
#include <string>

namespace pmt::util::smrt {

class GenericId {
 public:
  using IdType = uint64_t;

  enum : IdType {
    IdUninitialized = -1ULL,  // Underflow
    IdDefault = -2ULL,
    IdEoi = -3ULL,
    IdStart = -4ULL,
    IdReserved2 = -5ULL,
    IdReserved3 = -6ULL,
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
};

}  // namespace pmt::util::smrt