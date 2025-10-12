#pragma once

#include "pmt/parser/grammar/charset_literal.hpp"

#include <string>

namespace pmt::parser::grammar {

auto charset_literal_to_printable_string(CharsetLiteral const& charset_literal_) -> std::string;

}