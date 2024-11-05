#pragma once

#include "pmt/util/parse/generic_ast.hpp"

#include <optional>
#include <string>

namespace pmt::util::parse {

class CommentInfo {
 public:
  std::string _literal_start;
  std::string _literal_end;
};

class Terminal {
 public:
  std::optional<CommentInfo> _comment_info;
  std::string _name;
  GenericAst::id_type _id;
};

}  // namespace pmt::util::parse