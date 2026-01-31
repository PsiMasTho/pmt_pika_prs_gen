#pragma once

#include "pmt/fw_decl.hpp"

PMT_FW_DECL_NS_CLASS(pmt::meta, Grammar);

namespace pmt::meta {

class GrammarPruner {
public:
 // -$ Functions $-
 // --$ Other $--
 static void prune_grammar(Grammar& grammar_);
};
}