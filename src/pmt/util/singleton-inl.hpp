// clang-format off
#ifdef __INTELLISENSE__
    #include "pmt/util/singleton.hpp"
#endif
// clang-format on

#include <mutex>

namespace pmt::util {

template <std::default_initializable T_>
auto Singleton<T_>::instance() -> SharedHandle {
 static std::weak_ptr<T_> instance;
 static std::mutex mutex;

 std::scoped_lock l(mutex);

 if (!instance.expired())
  return instance.lock();

 SharedHandle ret = std::make_shared<T_>();
 instance = ret;
 return ret;
}

}  // namespace pmt::util