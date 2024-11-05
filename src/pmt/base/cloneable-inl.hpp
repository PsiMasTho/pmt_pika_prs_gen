// clang-format off
#ifdef __INTELLISENSE__
    #include "pmt/base/cloneable.hpp"
#endif
// clang-format on

namespace pmt::base {
template <typename CTRP_, typename BASE_>
auto InheritWithVClone<CTRP_, BASE_>::v_clone() const -> std::unique_ptr<Cloneable> {
  return std::make_unique<CTRP_>(*static_cast<CTRP_ const*>(this));
}
}  // namespace pmt::base