#pragma once

#include "pmt/meta/grammar.hpp"

#include <string_view>

namespace pmt::meta {

auto load_grammar(std::string_view input_grammar_) -> Grammar;

}