#pragma once

#include "pmt/base/dynamic_bitset.hpp"
#include "pmt/util/parsect/fa.hpp"
#include "pmt/util/parsect/fa_sink_wrapper.hpp"
#include "pmt/util/parsert/generic_lexer_tables.hpp"

#include <vector>

namespace pmt::parserbuilder {

class LexerTableTransitionConverter {
  class Context;

 public:
  static void convert(pmt::util::parsect::Fa const& fa_, pmt::util::parsert::GenericLexerTables& lexer_tables_);

 private:
  static void step_1(Context& context_);
  static void step_2(Context& context_);
  static void step_3(Context& context_);
  static void step_4(Context& context_);
  static void step_5(Context& context_);

  static auto debug_post_checks(Context const& context_) -> bool;

  static auto get_next_state(pmt::util::parsect::Fa const& fa_, pmt::util::parsect::Fa::StateNrType state_nr_, pmt::util::parsect::Fa::SymbolType symbol_) -> pmt::util::parsect::Fa::StateNrType;
};

class LexerTableTransitionConverter::Context {
 public:
  Context(pmt::util::parsect::Fa const& fa_, pmt::util::parsert::GenericLexerTables& lexer_tables_);

  pmt::util::parsect::FaSinkWrapper _fa_with_sink;
  std::vector<pmt::util::parsect::Fa::StateNrType> _ordering;
  std::vector<pmt::base::DynamicBitset> _diff_mat_2d;
  pmt::util::parsert::GenericLexerTables& _tables;
};

}  // namespace pmt::parserbuilder