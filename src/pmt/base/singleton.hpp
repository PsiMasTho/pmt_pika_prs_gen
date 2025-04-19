#pragma once

#include "pmt/namespace_id.hpp"

#include <concepts>
#include <memory>

namespace pmt::base {

template <std::default_initializable T_, uint64_t ID_ = NamespaceId::SharedNamespaceId, uint64_t EXTRA_ = 0>
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

}  // namespace pmt::base

#include "pmt/base/singleton-inl.hpp"