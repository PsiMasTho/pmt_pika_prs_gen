#pragma once

#include "pmt/container/interval_set.hpp"
#include "pmt/fw_decl.hpp"
#include "pmt/meta/charset_literal.hpp"

PMT_FW_DECL_NS_CLASS(pmt::meta, CharsetLiteral);

namespace pmt::meta {

// Characters that don't need to be written using integer literals.
// In other words the ones that you can write directly between quotes
auto get_string_body_characters() -> pmt::container::IntervalSet<pmt::rt::SymbolType> const&;

// A single character printed as an integer literal of value symbol_ printed in base base_
// in the form base_#symbol_ e.g. 10#97 for the ascii letter 'a'.
// Supports bases [2, 36]
auto character_to_integer_literal(pmt::rt::SymbolType symbol_, size_t base_ = 10) -> std::string;

// A *single* character to either a character literal or integer literal as it would appear in a grammar
// Printed as a quoted string if possible, otherwise as a base 10 integer literal
auto character_to_grammar_string(pmt::rt::SymbolType symbol_) -> std::string;

// A single character interval as it would appear in a charset expression in a grammar, in the simplest form possible:
// as a single quoted character or a double dot seperated interval of integer literals in the worst case. No braces
auto interval_to_range_grammar_string(pmt::container::Interval<pmt::rt::SymbolType> interval_) -> std::string;

// As it would appear in a grammar, in the simplest form possible:
// as a single quoted character or integer literal or in the worst case comma seperated ranges
auto symbol_set_to_grammar_string(CharsetLiteral::SetType const& charset_) -> std::string;

// As it would appear in a grammar in the simplest form possible:
// as a single quoted string or in the worst case as space seperated charsets
auto charset_literal_to_grammar_string(CharsetLiteral const& charset_literal_) -> std::string;

}  // namespace pmt::meta