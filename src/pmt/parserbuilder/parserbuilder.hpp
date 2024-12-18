#pragma once

#include <string>
#include <string_view>

namespace pmt::parserbuilder {

class ParserBuilder {
 public:
  explicit ParserBuilder(std::string_view input_grammar_path_, std::string_view input_sample_path_);

  void build();

 private:
  std::string _input_grammar;
  std::string _input_sample;
};

}  // namespace pmt::parserbuilder