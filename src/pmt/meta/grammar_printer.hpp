#pragma once

#include "pmt/fw_decl.hpp"

#include <ostream>

PMT_FW_DECL_NS_CLASS(pmt::meta, Grammar);

namespace pmt::meta {

class GrammarPrinter {
public:
 // -$ Types / Constants $-
 static void print(Grammar const& grammar_, std::ostream& out_);
};

}  // namespace pmt::meta