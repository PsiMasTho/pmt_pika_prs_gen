// clang-format off
#ifdef __INTELLISENSE__
    #include "pmt/base/hash.hpp"
#endif
// clang-format on

#include <functional>

namespace pmt::base {

template <typename T_>
void Hash::combine(const T_& v_, size_t& seed_) {
  combine(std::hash<T_>()(v_), seed_);
}

}  // namespace pmt::base