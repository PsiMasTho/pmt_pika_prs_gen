#include "pmt/base/cloneable.hpp"

namespace pmt::base {

CloneError::CloneError()
 : std::logic_error{"clone error"} {
}

auto Cloneable::clone() const -> std::unique_ptr<Cloneable> {
  return v_clone();
}

auto Cloneable::v_clone() const -> std::unique_ptr<Cloneable> {
  throw CloneError{};
}

}  // namespace pmt::base