// clang-format off
#ifdef __INTELLISENSE__
    #include "pmt/unreachable.hpp"
#endif
// clang-format on

#include <cassert>

namespace pmt {

static inline void unreachable() {
#if !defined NDEBUG || defined _DEBUG
 assert(false && "UNREACHABLE");
#endif

#if defined(_MSC_VER) && !defined(__clang__)  // MSVC
 __assume(false);
#else  // GCC, Clang
 __builtin_unreachable();
#endif
}

}  // namespace pmt