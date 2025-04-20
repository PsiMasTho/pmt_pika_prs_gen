#pragma once

#include "pmt/base/bitset.hpp"
#include "pmt/util/smrt/generic_id.hpp"
#include "pmt/util/smrt/tables_base.hpp"
#include "pmt/util/smrt/transition_tables.hpp"

#include <string>
#include <vector>

namespace pmt::util::smrt {

class GenericLexerTables : public TablesBase {
 public:
  // - Nonstatic data -
  // -- States --
  TransitionTables _transition_tables;

  // -- Accepts --
  std::vector<pmt::base::Bitset> _accepts;     // size: _state_count, all the same width
  std::vector<GenericId::IdType> _accept_ids;  // size: _accepts_width

  // -- Strings --
  std::vector<std::string> _terminal_names;  // size: _accepts_width
  std::vector<std::string> _id_names;        // size: (max(_accept_ids) excluding GenericId::*) + 1

  // -- Sizes --
  IndexType _accepts_width;

  // - Member functions -
  auto get_id_names_size() const -> size_t;
};

}  // namespace pmt::util::smrt