#pragma once

#include "pmt/fw_decl.hpp"

#include <iosfwd>

PMT_FW_DECL_NS_CLASS(pmt::util::parse, Fa);

namespace pmt::util::parse {

class GraphWriter {
 public:
  void write_dot(std::ostream& os_, Fa const& fa_) const;

 private:
};

}  // namespace pmt::util::parse