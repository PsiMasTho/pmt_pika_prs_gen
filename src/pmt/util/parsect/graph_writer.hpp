#pragma once

#include "pmt/util/parsect/state_machine.hpp"

#include <functional>
#include <iosfwd>
#include <set>

namespace pmt::util::parsect {

template <IsStateTag TAG_>
class GraphWriter {
 public:
  using AcceptsToLabel = std::function<std::string(size_t)>;

  static void write_dot(std::ostream& os_, StateMachine<TAG_> const& state_machine_, AcceptsToLabel accepts_to_label_ = nullptr);

 private:
  static auto accepts_to_label_default(size_t accepts_) -> std::string;
  static auto create_label(std::set<StateBase::SymbolType> const& state_nrs_) -> std::string;
  static auto is_displayable(StateBase::SymbolType sym_) -> bool;
  static auto to_displayable(StateBase::SymbolType sym_) -> std::string;
};

}  // namespace pmt::util::parsect