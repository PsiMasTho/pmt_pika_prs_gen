#pragma once

#include "pmt/base/dynamic_bitset.hpp"
#include "pmt/util/parsert/generic_lexer_tables.hpp"

#include <string>
#include <vector>

namespace pmt::parserbuilder {

class LexerTables {
 public:
  // - Data -
  // -- Special state numbers --
  pmt::util::parsert::GenericLexerTables::TableIndexType _state_nr_sink;
  pmt::util::parsert::GenericLexerTables::TableIndexType _state_nr_min_diff;

  // -- Paddings --
  pmt::util::parsert::GenericLexerTables::TableIndexType _padding_l;
  pmt::util::parsert::GenericLexerTables::TableIndexType _padding_r;

  // -- Sizes --
  pmt::util::parsert::GenericLexerTables::TableIndexType _accepts_2d_width;

  // -- Transitions --
  std::vector<pmt::util::parsert::GenericLexerTables::StateTransitionEntry> _state_transition_entries;
  std::vector<pmt::util::parsert::GenericLexerTables::CompressedTransitionEntry> _compressed_transition_entries;

  // -- Accepts --
  std::vector<pmt::util::parsert::GenericLexerTables::TableIndexType> _accepts_2d;
  std::vector<pmt::util::parsert::GenericId::IdType> _accept_ids;

  // -- Strings --
  std::vector<std::string> _terminal_names;
  std::vector<std::string> _id_names;

  // -- String views --
  std::vector<std::string_view> _terminal_names_sv;
  std::vector<std::string_view> _id_names_sv;

  // - Member functions -
  auto fill_sv() -> void;
  auto as_generic_lexer_tables() const -> pmt::util::parsert::GenericLexerTables;
};

}  // namespace pmt::parserbuilder