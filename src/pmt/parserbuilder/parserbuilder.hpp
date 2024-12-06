#pragma once

#include <set>
#include <string>
#include <string_view>

namespace pmt::parserbuilder {

class ParserBuilder {
 public:
  explicit ParserBuilder(std::string_view input_grammar_path_, std::string_view input_sample_path_, std::set<std::string> const& terminals_);

  void build();

 private:
  std::set<std::string> _terminals;
  std::string _input_grammar;
  std::string _input_sample;
};

}  // namespace pmt::parserbuilder