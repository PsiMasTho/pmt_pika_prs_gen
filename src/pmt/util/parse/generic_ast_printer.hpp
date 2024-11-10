#pragma once

#include "pmt/util/parse/generic_ast.hpp"

#include <functional>
#include <iosfwd>
#include <string>

namespace pmt::util::parse {

class GenericAstPrinter {
 public:
  using IdNameFunctionType = std::function<std::string(GenericAst::IdType)>;
  static inline size_t const INDENT_WIDTH = 2;

  explicit GenericAstPrinter(IdNameFunctionType id_name_function_);
  void print(GenericAst const& ast_, std::ostream& out_);

 private:
  auto get_name(GenericAst::IdType id_) -> std::string;

  IdNameFunctionType _id_name_function;
};

}  // namespace pmt::util::parse