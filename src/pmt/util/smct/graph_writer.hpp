#pragma once

#include "pmt/util/smct/state_machine.hpp"

#include <functional>
#include <iosfwd>
#include <set>

namespace pmt::util::smct {

class GraphWriter {
 public:
  using AcceptsToLabel = std::function<std::string(size_t)>;

  static void write_dot(std::ostream& os_, StateMachine const& state_machine_, AcceptsToLabel accepts_to_label_ = nullptr);

 private:
  static auto accepts_to_label_default(size_t accepts_) -> std::string;
  static auto create_label(std::set<Symbol::ValueType> const& state_nrs_) -> std::string;
  static auto is_displayable(Symbol::ValueType sym_) -> bool;
  static auto to_displayable(Symbol::ValueType sym_) -> std::string;
};

}  // namespace pmt::util::smct