#pragma once

#include "pmt/fw_decl.hpp"

#include <ostream>

PMT_FW_DECL_NS_CLASS(pmt::parser::builder, PikaProgram);

namespace pmt::parser::builder {

class PikaProgramPrinter {
public:
 // -$ Types / Constants $-
 class Args {
 public:
  std::ostream& _out;
  pmt::parser::builder::PikaProgram const& _program;
 };

 static void print(Args args_);
};

}  // namespace pmt::parser::builder