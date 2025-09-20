#pragma once

#include "pmt/fw_decl.hpp"

PMT_FW_DECL_NS_CLASS(pmt::parser::grammar, Grammar)

namespace pmt::parser::grammar {

class GrammarSimplifier {
public:
 // -$ Types / Constants $-
 class Args {
 public:
  Grammar& _grammar;
 };

 // -$ Functions $-
 // --$ Other $--
 static void simplify(Args args_);
};

}  // namespace pmt::parser::grammar