#pragma once

#include "pmt/fw_decl.hpp"

PMT_FW_DECL_NS_CLASS(pmt::meta, Grammar)

namespace pmt::meta {

class GrammarFlattener {
public:
 // -$ Functions $-
 // --$ Other $--
 static void flatten_grammar(Grammar& grammar_);
};

}  // namespace pmt::meta