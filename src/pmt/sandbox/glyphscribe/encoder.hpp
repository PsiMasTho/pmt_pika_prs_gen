#pragma once

#include "pmt/fw_decl.hpp"

#include <array>
#include <string>

PMT_FW_DECL_NS_CLASS(pmt::sandbox::glyphscribe, RuleSet)

namespace pmt::sandbox::glyphscribe {

class Encoder {
public:
 // -$ Types / Constants $-
 class EncodedWord {
 public:
  std::string _str;
  double _cost;
 };

 class EncodingCosts {
 public:
  double _ligature_cost_first;  // Cost of writing the cheapest ligature
  double _ligature_cost_incr;   // How much the cost increases by for each ligature after the 0th
  double _omitted_cost_first;   // Cost of writing the cheapest omitted letter
  double _omitted_cost_incr;    // How much the cost increases by for each omitted letter after the 0th
 };

 static inline std::array<char, 10> ENCODED_LIGATURE_GLYPHS = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9'};
 static inline std::array<char const*, 6> ENCODED_OMITTED_GLYPHS = {"'", "`", "''", "``", "'''", "```"};

 enum {
  MaxLigatures = ENCODED_LIGATURE_GLYPHS.size(),
  MaxOmitted = ENCODED_OMITTED_GLYPHS.size(),
 };

private:
 // -$ Data $-
 RuleSet const& _ruleset;
 EncodingCosts _encoding_cost;

public:
 explicit Encoder(RuleSet const& rule_set_, EncodingCosts encoding_costs_);

 auto encode(std::string const& str_) -> EncodedWord;
};

}  // namespace pmt::sandbox::glyphscribe