#include "pmt/meta/charset_literal.hpp"

#include "pmt/hash.hpp"

#include <cassert>
#include <unordered_set>

namespace pmt::meta {
using namespace pmt::container;
using namespace pmt::rt;

namespace {
auto is_symbol_printable(SymbolType symbol_) -> bool {
 // clang-format off
 static std::unordered_set<SymbolType> printable_symbols_ = {
  ' ', '!', '"', '#', '$', '%', '&', '\'', '(', ')', '*', '+', ',', '-', '.', '/',
  '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
  ':', ';', '<', '=', '>', '?', '@',
  'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M',
  'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
  '[', '\\', ']', '^', '_', '`',
  'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i',  'j',  'k',  'l',  'm',
  'n', 'o', 'p', 'q', 'r', 's',  't',  'u',  'v',  'w',  'x',  'y',  'z',
  '{', '|', '}', '~'
 };
 // clang-format on
 return printable_symbols_.contains(symbol_);
}

auto unprintable_symbol_to_string(SymbolType symbol_) {
 assert(!is_symbol_printable(symbol_));
 return "10#" + std::to_string(symbol_);
}

auto symbol_to_string(SymbolType symbol_) -> std::string {
 return is_symbol_printable(symbol_) ? ("\"" + std::string(1, symbol_) + "\"") : unprintable_symbol_to_string(symbol_);
}

auto interval_to_string(Interval<SymbolType> interval_) -> std::string {
 std::string ret;
 if (interval_.get_lower() == interval_.get_upper()) {  // One symbol interval
  ret += symbol_to_string(interval_.get_lower());
 } else {
  ret += symbol_to_string(interval_.get_lower()) + ".." + symbol_to_string(interval_.get_upper());
 }

 return ret;
}

auto symbol_set_to_string(CharsetLiteral::SymbolSet const& symbol_set_) -> std::string {
 if (symbol_set_.empty()) {
  return "_";
 }

 std::string ret;
 if (symbol_set_.size() == 1 && symbol_set_.get_by_index(0).get_lower() == symbol_set_.get_by_index(0).get_upper()) {  // One interval
  ret += interval_to_string(symbol_set_.get_by_index(0));
 } else {
  ret += "[";
  symbol_set_.for_each_interval([&](Interval<SymbolType> interval_) { ret += interval_to_string(interval_); });
  ret += "]";
 }
 return ret;
}
}  // namespace

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

void CharsetLiteral::push_back(Interval<SymbolType> symbol_interval_) {
 push_back(SymbolSet(symbol_interval_));
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

auto CharsetLiteral::to_string() const -> std::string {
 std::string ret;
 std::string delim;
 for (size_t i = 0; i < size(); ++i) {
  ret += std::exchange(delim, ", ") + symbol_set_to_string(get_symbol_set_at(i));
 }

 return ret;
}

}  // namespace pmt::meta