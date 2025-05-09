#pragma once

#include "pmt/fw_decl.hpp"
#include "pmt/parserbuilder/grammar_data.hpp"
#include "pmt/parserbuilder/parser_tables.hpp"

PMT_FW_DECL_NS_CLASS(pmt::util::smrt, GenericAst)

namespace pmt::parserbuilder {

class ParserTableBuilder {
 public:
  // -$ Functions $-
  // --$ Other $--
  auto build(pmt::util::smrt::GenericAst const& ast_, GrammarData const& grammar_data_) -> ParserTables;
};

}