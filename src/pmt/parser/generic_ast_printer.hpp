#pragma once

#include "pmt/fw_decl.hpp"
#include "pmt/parser/generic_id.hpp"

#include <functional>
#include <iosfwd>

PMT_FW_DECL_NS_CLASS(pmt::parser, GenericAst)

namespace pmt::parser {

class GenericAstPrinter {
 public:
 // -$ Types / Constants $-
 using IdToStringFnType = std::function<std::string(GenericId::IdType)>;

 class Args {
  public:
  IdToStringFnType _id_to_string_fn;
  std::ostream& _out;
  GenericAst const& _ast;
  size_t _indent_width = 2;
 };

 // --$ Other $--
 static void print(Args args_);
};

}  // namespace pmt::util::smrt