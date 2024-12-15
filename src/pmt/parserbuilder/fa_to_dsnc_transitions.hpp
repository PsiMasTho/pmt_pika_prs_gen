#pragma once

#include "pmt/base/dynamic_bitset.hpp"
#include "pmt/util/parsect/fa.hpp"
#include "pmt/util/parsert/generic_lexer_tables.hpp"

#include <vector>

namespace pmt::parserbuilder {

// Default-Shift-Next-Check Transition table
class Dsnc {
 public:
  pmt::util::parsert::GenericLexerTables::TableIndexType _state_nr_sink;
  pmt::util::parsert::GenericLexerTables::TableIndexType _state_nr_min_diff;

  pmt::util::parsert::GenericLexerTables::TableIndexType _padding_l = 0;

  std::vector<util::parsert::GenericLexerTables::StateTransitionEntry> _state_transition_entries;
  std::vector<util::parsert::GenericLexerTables::CompressedTransitionEntry> _compressed_transition_entries;
};

class FaToDsncTransitions {
 public:
  FaToDsncTransitions(pmt::util::parsect::Fa const& fa_);

  auto convert() -> Dsnc;

 private:
  void step_0(Dsnc& dsnc_);
  void step_1(Dsnc& dsnc_);
  void step_2(Dsnc& dsnc_);
  void step_3(Dsnc& dsnc_);
  void step_4(Dsnc& dsnc_);
  void step_5(Dsnc& dsnc_);

  auto debug_pre_checks() const -> bool;
  auto debug_post_checks(Dsnc const& dsnc_) const -> bool;

  static auto get_next_state(pmt::util::parsect::Fa const& fa_, pmt::util::parsect::Fa::StateNrType state_nr_, pmt::util::parsect::Fa::SymbolType symbol_) -> pmt::util::parsect::Fa::StateNrType;

  pmt::util::parsect::Fa _fa;
  std::vector<pmt::util::parsect::Fa::StateNrType> _ordering;
  std::vector<pmt::base::DynamicBitset> _diff_mat_2d;
};

}  // namespace pmt::parserbuilder