#pragma once

#include "pmt/fw_decl.hpp"
#include "pmt/util/parse/fa.hpp"

#include <functional>
#include <iosfwd>

PMT_FW_DECL_NS_CLASS(pmt::util::parse, Fa);

namespace pmt::util::parse {

class GraphWriter {
 public:
  using AcceptsToLabel = std::function<std::string(size_t)>;
  using SymbolToLabel = std::function<std::string(Fa::SymbolType)>;

  static void write_dot(std::ostream& os_, Fa const& fa_, AcceptsToLabel accepts_to_label_ = nullptr, SymbolToLabel symbol_to_label_ = nullptr);

 private:
  static auto accepts_to_label_default(size_t accepts_) -> std::string;
  static auto symbol_to_label_default(Fa::SymbolType symbol_) -> std::string;
};

}  // namespace pmt::util::parse