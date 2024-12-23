#pragma once

#include <cstddef>
#include <cstdint>
#include <unordered_map>
#include <vector>

namespace pmt::parserbuilder {

class AugmentedGrammar {
 public:
  class ProductionItem {
   public:
    enum Type : uint8_t {
      TerminalReference,
      RuleReference,
    };

    size_t _index;
    Type _reference_type;
  };

  using ProductionSequence = std::vector<ProductionItem>;
  using ProductionChoices = std::vector<ProductionSequence>;

  std::unordered_map<size_t, ProductionChoices> _productions;
};

}  // namespace pmt::parserbuilder