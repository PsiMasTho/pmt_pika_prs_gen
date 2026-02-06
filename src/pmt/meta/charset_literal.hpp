#pragma once

#include "pmt/container/interval_set.hpp"
#include "pmt/fw_decl.hpp"
#include "pmt/rt/primitives.hpp"

PMT_FW_DECL_NS_CLASS(pmt::rt, Ast)

namespace pmt::meta {

class CharsetLiteral {
public:
 // -$ Types / Constants $-
 using SetType = pmt::container::IntervalSet<pmt::rt::SymbolType>;
 using IntervalType = pmt::container::Interval<pmt::rt::SymbolType>;

private:
 // -$ Data $-
 std::vector<SetType> _literal;

public:
 // -$ Functions $-
 // --$ Lifetime $--
 explicit CharsetLiteral(pmt::rt::Ast const& ast_);
 CharsetLiteral() = default;

 // --$ Operators $--
 auto operator==(CharsetLiteral const& other_) const -> bool = default;
 auto operator!=(CharsetLiteral const& other_) const -> bool = default;

 // --$ Other $--
 auto get_symbol_set_at(size_t idx_) const -> SetType const&;
 auto get_symbol_set_at(size_t idx_) -> SetType&;

 void push_back(SetType symbol_set_);
 void push_back(IntervalType interval_);
 void push_back(pmt::rt::SymbolType symbol_);

 void pop_back();

 auto size() const -> size_t;
};

}  // namespace pmt::meta

namespace std {
template <>
struct hash<pmt::meta::CharsetLiteral> {
 auto operator()(pmt::meta::CharsetLiteral const& literal_) const -> size_t;
};
}  // namespace std