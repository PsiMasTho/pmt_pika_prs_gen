// clang-format off
#ifdef __INTELLISENSE__
    #include "pmt/base/hashable.hpp"
#endif
// clang-format on

namespace pmt::base {

template <typename CRTP_>
auto Hashable<CRTP_>::hash() const -> size_t {
  static_assert(std::is_base_of_v<Hashable<CRTP_>, CRTP_>);
  return CRTP_::hash(static_cast<CRTP_ const&>(*this));
}

}  // namespace pmt::base