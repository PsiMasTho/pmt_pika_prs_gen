#pragma once

#include "pmt/util/smrt/generic_ast.hpp"
#include "pmt/util/smrt/generic_id.hpp"

#include <functional>
#include <iosfwd>

namespace pmt::util::smrt {

class GenericAstPrinter {
 public:
  using IdToStringFnType = std::function<std::string(GenericId::IdType)>;
  static inline size_t const INDENT_WIDTH = 2;

  explicit GenericAstPrinter(IdToStringFnType id_to_string_fn_);
  void print(GenericAst const& ast_, std::ostream& out_);

 private:
  auto id_to_string(GenericId::IdType id_) -> std::string;

  IdToStringFnType _id_to_string_fn;
};

}  // namespace pmt::util::smrt