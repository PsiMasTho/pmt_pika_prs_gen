#include "pmt/util/parse/generic_ast.hpp"

#include <limits>

namespace pmt::util::parse {
generic_ast_base::id_type const generic_ast_base::UNINITIALIZED_ID = std::numeric_limits<id_type>::max();

auto generic_ast_base::get_id() const -> id_type {
  return _id;
}

void generic_ast_base::set_id(id_type id_) {
  _id = id_;
}
}  // namespace pmt::util::parse