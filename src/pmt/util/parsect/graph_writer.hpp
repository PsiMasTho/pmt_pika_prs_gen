#pragma once

#include "pmt/fw_decl.hpp"
#include "pmt/util/parsect/fa.hpp"

#include <functional>
#include <iosfwd>
#include <set>

PMT_FW_DECL_NS_CLASS(pmt::util::parsect, Fa);

namespace pmt::util::parsect {

class GraphWriter {
 public:
  using AcceptsToLabel = std::function<std::string(size_t)>;

  static void write_dot(std::ostream& os_, Fa const& fa_, AcceptsToLabel accepts_to_label_ = nullptr);

 private:
  static auto accepts_to_label_default(size_t accepts_) -> std::string;
  static auto create_label(std::set<Fa::SymbolType> const& state_nrs_) -> std::string;
  static auto is_displayable(Fa::SymbolType sym_) -> bool;
  static auto to_displayable(Fa::SymbolType sym_) -> std::string;
};

}  // namespace pmt::util::parsect