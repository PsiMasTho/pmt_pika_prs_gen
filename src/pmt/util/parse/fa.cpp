#include "pmt/util/parse/fa.hpp"

namespace pmt::util::parse {

auto Fa::get_unused_state_nr() const -> StateNrType {
  StateNrType ret = 0;
  while (_states.contains(ret)) {
    ++ret;
  }
  return ret;
}

}  // namespace pmt::util::parse