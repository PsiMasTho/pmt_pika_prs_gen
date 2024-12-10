#include "pmt/util/parsert/generic_id.hpp"

#include <stdexcept>

namespace pmt::util::parsert {

auto GenericId::id_to_string(IdType id_) -> std::string_view {
  switch (id_) {
    case IdUninitialized:
      return "Uninitialized";
    case IdDefault:
      return "Default";
    case IdEoi:
      return "Eoi";
    default:
      throw std::runtime_error("Invalid id");
  }
}

auto GenericId::string_to_id(std::string_view str_) -> IdType {
  if (str_ == "Uninitialized") {
    return IdUninitialized;
  } else if (str_ == "Default") {
    return IdDefault;
  } else if (str_ == "Eoi") {
    return IdEoi;
  } else {
    throw std::runtime_error("Invalid id");
  }
}

auto GenericId::is_generic_id(IdType id_) -> bool {
  switch (id_) {
    case IdUninitialized:
    case IdDefault:
    case IdEoi:
      return true;
    default:
      return false;
  }
}

}  // namespace pmt::util::parsert