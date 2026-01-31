// clang-format off
#ifdef __INTELLISENSE__
    #include "pmt/hash.hpp"
#endif
// clang-format on

#include <functional>

namespace pmt {

inline void Hash::combine(size_t hash_, size_t& seed_) {
 seed_ ^= (hash_ + Phi64 + (seed_ << 6) + (seed_ >> 2));
}

template <typename T_>
void Hash::combine(const T_& v_, size_t& seed_) {
 combine(std::hash<T_>()(v_), seed_);
}

}  // namespace pmt