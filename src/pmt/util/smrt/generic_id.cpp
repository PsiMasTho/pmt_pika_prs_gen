#include "pmt/util/smrt/generic_id.hpp"

#include <stdexcept>

namespace pmt::util::smrt {

auto GenericId::id_to_string(IdType id_) -> std::string {
  switch (id_) {
    case IdUninitialized:
      return "IdUninitialized";
    case IdDefault:
      return "IdDefault";
    case IdEoi:
      return "IdEoi";
    case IdStart:
      return "IdStart";
    case IdReserved2:
      return "IdReserved2";
    case IdReserved3:
      return "IdReserved3";
    case IdReserved4:
      return "IdReserved4";
    case IdReserved5:
      return "IdReserved5";
    case IdReserved6:
      return "IdReserved6";
    case IdReserved7:
      return "IdReserved7";
    case IdReserved8:
      return "IdReserved8";
    case IdReserved9:
      return "IdReserved9";
    default:
      throw std::runtime_error("Invalid id");
  }
}

auto GenericId::string_to_id(std::string_view str_) -> IdType {
  for (IdType i = IdUninitialized; i >= IdReserved9; --i) {
    if (id_to_string(i) == str_) {
      return i;
    }
  }

  throw std::runtime_error("Invalid id");
}

auto GenericId::is_generic_id(IdType id_) -> bool {
  return id_ >= IdReserved9;
}

}  // namespace pmt::util::smrt