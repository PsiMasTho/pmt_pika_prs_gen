#pragma once

#include "pmt/meta/charset.hpp"
#include "pmt/rt/primitives.hpp"

namespace pmt::meta {

class CharsetLiteral {
public:
 // -$ Types / Constants $-
private:
 // -$ Data $-
 std::vector<Charset::SetType> _literal;

public:
 // -$ Functions $-
 // --$ Operators $--
 auto operator==(CharsetLiteral const& other_) const -> bool = default;
 auto operator!=(CharsetLiteral const& other_) const -> bool = default;

 // --$ Other $--
 auto hash() const -> size_t;

 auto get_symbol_set_at(size_t idx_) const -> Charset::SetType const&;
 auto get_symbol_set_at(size_t idx_) -> Charset::SetType&;

 void push_back(Charset::SetType symbol_set_);
 void push_back(pmt::container::Interval<pmt::rt::SymbolType> symbol_interval_);
 void push_back(pmt::rt::SymbolType symbol_);

 void pop_back();

 auto size() const -> size_t;
};

}  // namespace pmt::meta