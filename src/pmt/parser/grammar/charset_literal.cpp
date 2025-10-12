#include "pmt/parser/grammar/charset_literal.hpp"

#include "pmt/base/hash.hpp"

namespace pmt::parser::grammar {

auto CharsetLiteral::hash() const -> size_t {
 size_t seed = pmt::base::Hash::Phi64;

 for (size_t i = 0; i < size(); ++i) {
  pmt::base::Hash::combine(get_symbol_set_at<IsHidden::No>(i), seed);
  pmt::base::Hash::combine(get_symbol_set_at<IsHidden::Yes>(i), seed);
 }

 return seed;
}

void CharsetLiteral::pop_back() {
 _literal.pop_back();
}

auto CharsetLiteral::size() const -> size_t {
 return _literal.size();
}

}  // namespace pmt::parser::grammar