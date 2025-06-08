#include "pmt/parser/generic_id.hpp"

#include <stdexcept>
#include <regex>

namespace pmt::parser {

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
    case IdNewline:
      return "IdNewline";
    case IdRoot:
      return "IdRoot";
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

auto GenericId::is_generic_id(std::string_view str_) -> bool {
 static const std::regex generic_id_regex = std::regex(R"(^Id(?:Uninitialized|Default|Eoi|Start|Root|Newline|Reserved\d+)$)", std::regex::optimize);
  return std::regex_match(str_.data(), generic_id_regex);
}

}  // namespace pmt::util::smrt