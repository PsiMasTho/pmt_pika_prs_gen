#pragma once

#include "pmt/base/dynamic_bitset.hpp"
#include "pmt/util/parsert/generic_fa_state_tables.hpp"
#include "pmt/util/parsert/generic_id.hpp"
#include "pmt/util/parsert/generic_tables_base.hpp"

#include <string>
#include <vector>

namespace pmt::util::parsert {

class GenericLexerTables : public GenericTablesBase {
 public:
  // - Nonstatic data -
  // -- States --
  GenericFaStateTables _fa_state_tables;

  // -- Accepts --
  std::vector<pmt::base::DynamicBitset> _accepts;  // size: _state_count, all the same width
  std::vector<GenericId::IdType> _accept_ids;      // size: _accepts_width

  // -- Strings --
  std::vector<std::string> _terminal_names;  // size: _accepts_width
  std::vector<std::string> _id_names;        // size: (max(_accept_ids) excluding GenericId::*) + 1

  // -- Comments --
  pmt::base::DynamicBitset _comment_accepts;

  // -- Whitespace --
  std::vector<TableIndexType> _whitespace;

  // -- Sizes --
  TableIndexType _accepts_width;

  // - Member functions -
  auto get_id_names_size() const -> size_t;
};

}  // namespace pmt::util::parsert