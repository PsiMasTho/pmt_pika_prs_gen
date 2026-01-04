#pragma once

#include "pmt/base/interval_set.hpp"
#include "pmt/fw_decl.hpp"
#include "pmt/sm/primitives.hpp"

PMT_FW_DECL_NS_CLASS(pmt::parser, GenericAst)

namespace pmt::parser::grammar {

class Charset {
public:
 // -$ Types / Constants $-
 using SetType = pmt::base::IntervalSet<pmt::sm::SymbolType>;

private:
 // -$ Data $-
 SetType _values;

public:
 // -$ Functions $-
 // --$ Lifetime $--
 Charset() = default;
 explicit Charset(pmt::parser::GenericAst const &ast_);

 // --$ Other $--
 [[nodiscard]] auto get_values() const -> SetType const &;
};

}  // namespace pmt::parser::grammar
