#pragma once

#include "pmt/fw_decl.hpp"

#include <ostream>

PMT_FW_DECL_NS_CLASS(pmt::parser::grammar, Grammar);

namespace pmt::parser::grammar {

class GrammarPrinter {
public:
 // -$ Types / Constants $-
 class Args {
 public:
  std::ostream& _out;
  Grammar const& _grammar;
 };

 static void print(Args args_);
};

}  // namespace pmt::parser::grammar