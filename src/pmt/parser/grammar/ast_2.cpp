#include "pmt/parser/grammar/ast_2.hpp"

#include <stdexcept>

namespace pmt::parser::grammar {
namespace {
char const* const ID_STRINGS[] = {
#include "pmt/parser/grammar/id_strings_2-inl.hpp"
};
}  // namespace

auto Ast2::id_to_string(GenericId::IdType id_) -> std::string {
 if (id_ < std::size(ID_STRINGS)) {
  return ID_STRINGS[id_];
 }

 throw std::runtime_error("Invalid id");
}
}  // namespace pmt::parser::grammar