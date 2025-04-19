// clang-format off
#ifdef __INTELLISENSE__
    #include "pmt/base/singleton.hpp"
#endif
// clang-format on

#include <mutex>

namespace pmt::base {

template <std::default_initializable T_, uint64_t ID_, uint64_t EXTRA_>
auto Singleton<T_, ID_, EXTRA_>::instance() -> SharedHandle {
  static std::weak_ptr<T_> instance;
  static std::mutex mutex;

  std::scoped_lock l(mutex);

  if (!instance.expired())
    return instance.lock();

  SharedHandle ret = std::make_shared<T_>();
  instance = ret;
  return ret;
}

}  // namespace pmt::base