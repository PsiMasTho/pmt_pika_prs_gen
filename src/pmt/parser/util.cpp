#include "pmt/parser/util.hpp"

#include "pmt/base/match.hpp"

namespace pmt::parser {
using namespace pmt::base;

auto is_symbol_printable(SymbolType symbol_) -> bool {
 return Match::is_in_any_of(symbol_, Interval('A', 'Z'), Interval('a', 'z'), Interval('0', '9'),
                            Interval(32),                                                                           // Space
                            Interval(33), Interval(35, 47), Interval(58, 64), Interval(91, 96), Interval(123, 126)  // Punctuation, except double quote
 );
}

}  // namespace pmt::parser