#pragma once

#include "pmt/fw_decl.hpp"

PMT_FW_DECL_NS_CLASS(pmt::meta, Grammar)

namespace pmt::meta {

class GrammarSimplifier {
public:
 // -$ Functions $-
 // --$ Other $--
 static void simplify(Grammar& grammar_);
};

}  // namespace pmt::meta