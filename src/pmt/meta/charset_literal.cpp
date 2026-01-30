#include "pmt/meta/charset_literal.hpp"

#include "pmt/hash.hpp"

#include <cassert>

namespace pmt::meta {
using namespace pmt::container;
using namespace pmt::rt;

namespace {}  // namespace

auto CharsetLiteral::get_symbol_set_at(size_t idx_) const -> Charset::SetType const& {
 assert(idx_ < size());
 return _literal[idx_];
}

auto CharsetLiteral::get_symbol_set_at(size_t idx_) -> Charset::SetType& {
 assert(idx_ < size());
 return _literal[idx_];
}

void CharsetLiteral::push_back(Charset::SetType symbol_set_) {
 _literal.emplace_back();
 get_symbol_set_at(size() - 1) = std::move(symbol_set_);
}

void CharsetLiteral::push_back(Interval<SymbolType> symbol_interval_) {
 push_back(Charset::SetType(symbol_interval_));
}

void CharsetLiteral::push_back(SymbolType symbol_) {
 push_back(Interval<SymbolType>(symbol_));
}

void CharsetLiteral::pop_back() {
 _literal.pop_back();
}

auto CharsetLiteral::size() const -> size_t {
 return _literal.size();
}

}  // namespace pmt::meta

namespace std {
auto hash<pmt::meta::CharsetLiteral>::operator()(pmt::meta::CharsetLiteral const& literal_) const -> size_t {
 size_t seed = pmt::Hash::Phi64;

 for (size_t i = 0; i < literal_.size(); ++i) {
  pmt::Hash::combine(literal_.get_symbol_set_at(i), seed);
  pmt::Hash::combine(literal_.get_symbol_set_at(i), seed);
 }

 return seed;
}
}  // namespace std