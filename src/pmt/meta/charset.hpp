#pragma once

#include "pmt/container/interval_set.hpp"
#include "pmt/fw_decl.hpp"
#include "pmt/rt/primitives.hpp"

PMT_FW_DECL_NS_CLASS(pmt::rt, Ast)

namespace pmt::meta {

class Charset {
public:
 // -$ Types / Constants $-
 using SetType = pmt::container::IntervalSet<pmt::rt::SymbolType>;

private:
 // -$ Data $-
 SetType _values;

public:
 // -$ Functions $-
 // --$ Lifetime $--
 Charset() = default;
 explicit Charset(pmt::rt::Ast const &ast_);

 // --$ Other $--
 [[nodiscard]] auto get_values() const -> SetType const &;
};

}  // namespace pmt::meta
