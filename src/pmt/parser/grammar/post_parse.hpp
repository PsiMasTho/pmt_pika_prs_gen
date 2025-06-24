#pragma once

#include "pmt/fw_decl.hpp"

PMT_FW_DECL_NS_CLASS(pmt::parser, GenericAst);

namespace pmt::parser::grammar {

class PostParse {
 // -$ Types / Constants $-
public:
 class Args {
 public:
  pmt::parser::GenericAst& _ast_root;
 };

 // -$ Functions $-
 // --$ Other $--
 static void transform(Args args_);
};

}  // namespace pmt::parser::grammar