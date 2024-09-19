#include "pmt/util/parse/generic_ast.hpp"

namespace pmt::util::parse {

auto generic_ast_base::get_id() const -> id_type {
  return _id;
}

void generic_ast_base::set_id(id_type id_) {
  _id = id_;
}
}  // namespace pmt::util::parse