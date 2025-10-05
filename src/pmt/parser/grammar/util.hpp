#pragma once

#include "pmt/base/interval_set.hpp"
#include "pmt/parser/primitives.hpp"

#include <string>

namespace pmt::parser::grammar {

auto literal_sequence_to_printable_string(std::vector<pmt::base::IntervalSet<SymbolType>> const& literal_) -> std::string;

}