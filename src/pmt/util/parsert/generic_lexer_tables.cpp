#include "pmt/util/parsert/generic_lexer_tables.hpp"

#include <cassert>

namespace pmt::util::parsert {
auto GenericLexerTables::get_id_names_size() const -> size_t {
  GenericId::IdType max_id = 0;

  for (TableIndexType i = 0; i < _accepts_width; ++i) {
    max_id = (GenericId::is_generic_id(_accept_ids[i])) ? max_id : std::max(max_id, _accept_ids[i]);
  }

  return max_id + 1;
}

}  // namespace pmt::util::parsert