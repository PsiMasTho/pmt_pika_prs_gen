#include "pmt/pika/rt/reserved_ids.hpp"

#include <stdexcept>

namespace pmt::pika::rt {

auto ReservedIds::id_to_string(IdType id_) -> std::string {
 switch (id_) {
  case IdUninitialized:
   return "IdUninitialized";
  case IdDefault:
   return "IdDefault";
  case IdRoot:
   return "IdRoot";
  case IdTmpEpsilon:
   return "IdTmpEpsilon";
  case IdInvalid:
   return "IdInvalid";
  default:
   throw std::runtime_error("Invalid id value: " + std::to_string(id_));
 }
}

auto ReservedIds::string_to_id(std::string_view str_) -> IdType {
 for (IdType i = IdUninitialized; i >= IdInvalid; --i) {
  if (id_to_string(i) == str_) {
   return i;
  }
 }

 throw std::runtime_error("Invalid id string: " + std::string(str_));
}

auto ReservedIds::is_reserved_id(IdType id_) -> bool {
 return id_ >= IdInvalid;
}

auto ReservedIds::is_reserved_id(std::string_view str_) -> bool {
 return (str_ == "IdUninitialized" || str_ == "IdDefault" || str_ == "IdRoot" || str_ == "IdTmpEpsilon" || str_ == "IdInvalid");
}

}  // namespace pmt::pika::rt