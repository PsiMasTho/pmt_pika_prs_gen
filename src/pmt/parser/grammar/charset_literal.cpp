#include "pmt/parser/grammar/charset_literal.hpp"

#include "pmt/base/hash.hpp"

#include <cassert>

namespace pmt::parser::grammar {
using namespace pmt::base;

auto CharsetLiteral::hash() const -> size_t {
 size_t seed = Hash::Phi64;

 for (size_t i = 0; i < size(); ++i) {
  Hash::combine(get_symbol_set_at(i), seed);
  Hash::combine(get_symbol_set_at(i), seed);
 }

 return seed;
}

auto CharsetLiteral::get_symbol_set_at(size_t idx_) const -> SymbolSet const& {
 assert(idx_ < size());
 return _literal[idx_];
}

auto CharsetLiteral::get_symbol_set_at(size_t idx_) -> SymbolSet& {
 assert(idx_ < size());
 return _literal[idx_];
}

void CharsetLiteral::push_back(SymbolSet symbol_set_) {
 _literal.emplace_back();
 get_symbol_set_at(size() - 1) = std::move(symbol_set_);
}

void CharsetLiteral::push_back(Interval<pmt::util::sm::SymbolValueType> symbol_interval_) {
 push_back(SymbolSet(symbol_interval_));
}

void CharsetLiteral::push_back(pmt::util::sm::SymbolValueType symbol_) {
 push_back(Interval<pmt::util::sm::SymbolValueType>(symbol_));
}

void CharsetLiteral::push_back() {
 _literal.emplace_back();
}

void CharsetLiteral::pop_back() {
 _literal.pop_back();
}

auto CharsetLiteral::is_hidden() const -> bool {
 return _is_hidden;
}

void CharsetLiteral::set_hidden(bool value_) {
 _is_hidden = value_;
}

auto CharsetLiteral::size() const -> size_t {
 return _literal.size();
}

}  // namespace pmt::parser::grammar