#include "pmt/meta/language.hpp"

#include <stdexcept>

namespace pmt::meta {
namespace {
char const* const ID_STRINGS[] = {
#include "pmt/meta/id_strings-inl.hpp"
};
}  // namespace

auto Language::id_to_string(pmt::rt::AstId::IdType id_) -> std::string {
 if (id_ < std::size(ID_STRINGS)) {
  return ID_STRINGS[id_];
 }

 throw std::runtime_error("Invalid id");
}
}  // namespace pmt::meta