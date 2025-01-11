#include "pmt/base/cloneable.hpp"

namespace pmt::base {

CloneError::CloneError()
 : std::logic_error{"clone error"} {
}

}  // namespace pmt::base