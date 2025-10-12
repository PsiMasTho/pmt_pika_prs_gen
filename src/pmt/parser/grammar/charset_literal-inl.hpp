// clang-format off
#ifdef __INTELLISENSE__
    #include "pmt/parser/grammar/charset_literal.hpp"
#endif
// clang-format on

#include <cassert>

namespace pmt::parser::grammar {

template <CharsetLiteral::IsHidden IS_HIDDEN_>
auto CharsetLiteral::get_symbol_set_at(size_t idx_) const -> SymbolSet const& {
 assert(idx_ < size());
 return std::get<static_cast<size_t>(IS_HIDDEN_)>(_literal[idx_]);
}

template <CharsetLiteral::IsHidden IS_HIDDEN_>
auto CharsetLiteral::get_symbol_set_at(size_t idx_) -> SymbolSet& {
 assert(idx_ < size());
 return std::get<static_cast<size_t>(IS_HIDDEN_)>(_literal[idx_]);
}

template <CharsetLiteral::IsHidden IS_HIDDEN_>
void CharsetLiteral::push_back(SymbolSet symbol_set_) {
 _literal.emplace_back();
 get_symbol_set_at<IS_HIDDEN_>(size() - 1) = std::move(symbol_set_);
}

template <CharsetLiteral::IsHidden IS_HIDDEN_>
void CharsetLiteral::push_back(pmt::base::Interval<pmt::util::sm::SymbolValueType> symbol_interval_) {
 push_back<IS_HIDDEN_>(SymbolSet(symbol_interval_));
}

template <CharsetLiteral::IsHidden IS_HIDDEN_>
void CharsetLiteral::push_back(pmt::util::sm::SymbolValueType symbol_) {
 push_back<IS_HIDDEN_>(pmt::base::Interval<pmt::util::sm::SymbolValueType>(symbol_));
}

}  // namespace pmt::parser::grammar