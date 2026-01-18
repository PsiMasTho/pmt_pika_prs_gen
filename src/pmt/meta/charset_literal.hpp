#pragma once

#include "pmt/container/interval_set.hpp"
#include "pmt/rt/primitives.hpp"

namespace pmt::meta {

class CharsetLiteral {
public:
 // -$ Types / Constants $-
 using SymbolSet = pmt::container::IntervalSet<pmt::rt::SymbolType>;

private:
 // -$ Data $-
 std::vector<SymbolSet> _literal;

public:
 // -$ Functions $-
 // --$ Operators $--
 auto operator==(CharsetLiteral const& other_) const -> bool = default;
 auto operator!=(CharsetLiteral const& other_) const -> bool = default;

 // --$ Other $--
 auto hash() const -> size_t;

 auto get_symbol_set_at(size_t idx_) const -> SymbolSet const&;
 auto get_symbol_set_at(size_t idx_) -> SymbolSet&;

 void push_back(SymbolSet symbol_set_);
 void push_back(pmt::container::Interval<pmt::rt::SymbolType> symbol_interval_);
 void push_back(pmt::rt::SymbolType symbol_);

 void pop_back();

 auto size() const -> size_t;

 auto to_string() const -> std::string;
};

}  // namespace pmt::meta