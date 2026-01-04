#include "pmt/parser/util.hpp"

#include "pmt/base/match.hpp"

namespace pmt::parser {
using namespace pmt::base;
using namespace pmt::sm;

// -$ Todo $- Dotfile printing and other printing need different is_symbol_printable implementations
auto is_symbol_printable(SymbolType symbol_) -> bool {
 return Match::is_in_any_of(symbol_, Interval('A', 'Z'), Interval('a', 'z'), Interval('0', '9'), Interval(32), Interval(33), Interval(35, 37), Interval(40, 47), Interval(58, 59), Interval(61, 61), Interval(63, 64), Interval(91, 96), Interval(123, 126));
}

}  // namespace pmt::parser