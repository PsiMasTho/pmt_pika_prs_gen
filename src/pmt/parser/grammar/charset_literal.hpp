#pragma once

#include "pmt/base/hashable.hpp"
#include "pmt/base/interval_set.hpp"
#include "pmt/util/sm/primitives.hpp"

namespace pmt::parser::grammar {

class CharsetLiteral : public pmt::base::Hashable<CharsetLiteral> {
public:
 // -$ Types / Constants $-
 enum class IsHidden {
  No,
  Yes,
 };

 using SymbolSet = pmt::base::IntervalSet<pmt::util::sm::SymbolValueType>;

private:
 // -$ Data $-
 std::vector<SymbolSet> _literal;
 bool _is_hidden : 1 = false;
 ;

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
 void push_back(pmt::base::Interval<pmt::util::sm::SymbolValueType> symbol_interval_);
 void push_back(pmt::util::sm::SymbolValueType symbol_);
 void push_back();

 void pop_back();

 auto is_hidden() const -> bool;
 void set_hidden(bool value_);

 auto size() const -> size_t;
};

}  // namespace pmt::parser::grammar