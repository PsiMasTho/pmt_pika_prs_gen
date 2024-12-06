#pragma once

#include "pmt/fw_decl.hpp"
#include "pmt/util/parse/generic_ast.hpp"

#include <string_view>

PMT_FW_DECL_NS_CLASS(pmt::util::parse, GenericLexerTables);
PMT_FW_DECL_NS_CLASS(pmt::base, DynamicBitset);

namespace pmt::util::parse {

class GenericLexer {
 public:
  GenericLexer(std::string_view input_, GenericLexerTables const& tables_);

  auto next_token(pmt::base::DynamicBitset const& accepts_) -> GenericAst::UniqueHandle;

 private:
  std::string_view _input;
  GenericLexerTables const& _tables;
};

}  // namespace pmt::util::parse