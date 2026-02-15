#pragma once

#include "pmt/pika/rt/primitives.hpp"

#include <limits>
#include <string>

namespace pmt::pika::rt {

class ReservedIds {
public:
 // -$ Types / Constants $-
 enum : IdType {
  IdUninitialized = std::numeric_limits<IdType>::max(),
  IdDefault = IdUninitialized - 1,
  IdRoot = IdDefault - 1,
  IdTmpEpsilon = IdRoot - 1,
  IdInvalid = IdTmpEpsilon - 1,
 };

 static auto id_to_string(IdType id_) -> std::string;
 static auto string_to_id(std::string_view str_) -> IdType;

 static auto is_reserved_id(IdType id_) -> bool;
 static auto is_reserved_id(std::string_view str_) -> bool;
};

}  // namespace pmt::pika::rt