#pragma once

#include "pmt/base/hashable.hpp"
#include "pmt/base/interval_set.hpp"
#include "pmt/sm/primitives.hpp"

namespace pmt::parser::grammar {

class CharsetLiteral : public pmt::base::Hashable<CharsetLiteral> {
public:
 // -$ Types / Constants $-
 using SymbolSet = pmt::base::IntervalSet<pmt::sm::SymbolType>;

private:
 // -$ Data $-
 std::vector<SymbolSet> _literal;

public:
 // -$ Functions $-
 // --$ Operators $--
 auto operator==(CharsetLiteral const& other_) const -> bool = default;
 auto operator!=(CharsetLiteral const& other_) const -> bool = default;

 // --$ Inherited: pmt::base::Hashable $--
 auto hash() const -> size_t;

 // --$ Other $--
 auto get_symbol_set_at(size_t idx_) const -> SymbolSet const&;
 auto get_symbol_set_at(size_t idx_) -> SymbolSet&;

 void push_back(SymbolSet symbol_set_);
 void push_back(pmt::base::Interval<pmt::sm::SymbolType> symbol_interval_);
 void push_back(pmt::sm::SymbolType symbol_);

 void pop_back();

 auto size() const -> size_t;
};

}  // namespace pmt::parser::grammar