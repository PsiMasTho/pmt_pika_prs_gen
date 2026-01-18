#pragma once

#include "pmt/fw_decl.hpp"

#include <iosfwd>

PMT_FW_DECL_NS_CLASS(pmt::builder, PikaProgram);

namespace pmt::builder {

class PikaProgramPrinter {
public:
 // -$ Types / Constants $-
 static void print(PikaProgram const& program_, std::ostream& out_);
};

}  // namespace pmt::builder