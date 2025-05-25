#pragma once

#include "pmt/util/smrt/generic_ast.hpp"
#include "pmt/util/smrt/generic_id.hpp"

#include <functional>
#include <iosfwd>

namespace pmt::util::smrt {

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