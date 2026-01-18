#pragma once

#include <concepts>
#include <memory>

namespace pmt::util {

template <std::default_initializable T_>
class Singleton {
public:
 using ValueType = T_;
 using SharedHandle = std::shared_ptr<ValueType>;
 static auto instance() -> SharedHandle;
 ~Singleton() = default;

private:
 Singleton() = default;
 Singleton(Singleton const&) = delete;
 auto operator=(Singleton const&) -> Singleton& = delete;
};

}  // namespace pmt::util

#include "pmt/util/singleton-inl.hpp"