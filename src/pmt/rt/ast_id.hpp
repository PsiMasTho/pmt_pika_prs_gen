#pragma once

#include <cstdint>
#include <string>

namespace pmt::rt {

class AstId {
public:
 using IdType = uint64_t;

 enum : IdType {
  IdUninitialized = -1ULL,  // Underflow
  IdDefault = -2ULL,
  IdRoot = -3ULL,
 };

 static auto id_to_string(IdType id_) -> std::string;
 static auto string_to_id(std::string_view str_) -> IdType;

 static auto is_generic_id(IdType id_) -> bool;
 static auto is_generic_id(std::string_view str_) -> bool;
};

}  // namespace pmt::rt