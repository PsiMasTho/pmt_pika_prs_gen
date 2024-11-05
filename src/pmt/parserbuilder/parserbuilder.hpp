#pragma once

#include <string>
#include <string_view>

namespace pmt::parserbuilder {

class ParserBuilder {
 public:
  explicit ParserBuilder(std::string_view input_path_);

  void build();

 private:
  std::string _input;
};

}  // namespace pmt::parserbuilder