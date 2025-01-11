// clang-format off
#ifdef __INTELLISENSE__
    #include "pmt/base/cloneable.hpp"
#endif
// clang-format on

namespace pmt::base {

template <typename T_>
auto Cloneable<T_>::clone() const -> std::unique_ptr<T_> {
  throw CloneError{};
}

template <typename CTRP_, typename BASE_>
auto InheritWithClone<CTRP_, BASE_>::clone() const -> std::unique_ptr<BASE_> {
  return std::make_unique<BASE_>(*static_cast<CTRP_ const*>(this));
}

template <typename CTRP_, typename BASE_>
auto VirtualInheritWithClone<CTRP_, BASE_>::clone() const -> std::unique_ptr<BASE_> {
  return std::make_unique<BASE_>(*static_cast<CTRP_ const*>(this));
}

}  // namespace pmt::base