#pragma once

#include "pmt/base/hashable.hpp"
#include "pmt/base/interval_set.hpp"
#include "pmt/util/sm/primitives.hpp"

#include <utility>

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
 std::vector<std::pair<SymbolSet, SymbolSet>> _literal;

public:
 // -$ Functions $-

 // --$ Operators $--
 auto operator==(CharsetLiteral const& other_) const -> bool = default;
 auto operator!=(CharsetLiteral const& other_) const -> bool = default;

 // --$ Inherited: pmt::base::Hashable $--
 auto hash() const -> size_t;

 // --$ Other $--
 template <IsHidden IS_HIDDEN_>
 auto get_symbol_set_at(size_t idx_) const -> SymbolSet const&;

 template <IsHidden IS_HIDDEN_>
 auto get_symbol_set_at(size_t idx_) -> SymbolSet&;

 template <IsHidden IS_HIDDEN_>
 void push_back(SymbolSet symbol_set_);

 template <IsHidden IS_HIDDEN_>
 void push_back(pmt::base::Interval<pmt::util::sm::SymbolValueType> symbol_interval_);

 template <IsHidden IS_HIDDEN_>
 void push_back(pmt::util::sm::SymbolValueType symbol_);

 void pop_back();

 auto size() const -> size_t;
};

}  // namespace pmt::parser::grammar

#include "pmt/parser/grammar/charset_literal-inl.hpp"