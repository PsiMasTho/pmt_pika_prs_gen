#pragma once

#include "pmt/base/interval_set.hpp"
#include "pmt/fw_decl.hpp"
#include "pmt/util/smrt/state_machine_primitives.hpp"

#include <functional>
#include <iosfwd>

PMT_FW_DECL_NS_CLASS(pmt::util::smct, StateMachine)

namespace pmt::util::smct {

class GraphWriter {
 public:
  using AcceptsToLabel = std::function<std::string(size_t)>;

  static void write_dot(std::ostream& os_, StateMachine const& state_machine_, AcceptsToLabel accepts_to_label_ = nullptr);

 private:
  static auto accepts_to_label_default(size_t accepts_) -> std::string;
  static auto create_label(pmt::base::IntervalSet<pmt::util::smrt::SymbolType> const& symbols_) -> std::string;
  static auto is_displayable(pmt::util::smrt::SymbolType symbol_) -> bool;
  static auto to_displayable(pmt::util::smrt::SymbolType symbol_) -> std::string;
};

}  // namespace pmt::util::smct