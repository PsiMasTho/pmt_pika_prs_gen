#include "pmt/rt/ast_id.hpp"

#include <stdexcept>

namespace pmt::rt {

auto AstId::id_to_string(IdType id_) -> std::string {
 switch (id_) {
  case IdUninitialized:
   return "IdUninitialized";
  case IdDefault:
   return "IdDefault";
  case IdRoot:
   return "IdRoot";
  default:
   throw std::runtime_error("Invalid id value: " + std::to_string(id_));
 }
}

auto AstId::string_to_id(std::string_view str_) -> IdType {
 for (IdType i = IdUninitialized; i >= IdRoot; --i) {
  if (id_to_string(i) == str_) {
   return i;
  }
 }

 throw std::runtime_error("Invalid id string: " + std::string(str_));
}

auto AstId::is_generic_id(IdType id_) -> bool {
 return id_ >= IdRoot;
}

auto AstId::is_generic_id(std::string_view str_) -> bool {
 return (str_ == "IdUninitialized" || str_ == "IdDefault" || str_ == "IdRoot");
}

}  // namespace pmt::rt