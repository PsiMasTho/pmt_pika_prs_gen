#include "pmt/util/smrt/generic_lexer_tables.hpp"

#include <cassert>

namespace pmt::util::smrt {
auto GenericLexerTables::get_id_names_size() const -> size_t {
  GenericId::IdType max_id = 0;

  for (IndexType i = 0; i < _accepts_width; ++i) {
    max_id = (GenericId::is_generic_id(_accept_ids[i])) ? max_id : std::max(max_id, _accept_ids[i]);
  }

  return max_id + 1;
}

}  // namespace pmt::util::smrt