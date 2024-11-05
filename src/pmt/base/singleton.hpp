#pragma once

#include "pmt/namespace_id.hpp"

#include <concepts>
#include <memory>

namespace pmt::base {

template <std::default_initializable T_, uint64_t ID_ = namespace_id::SHARED_NAMESPACE_ID, uint64_t EXTRA_ = 0>
class Singleton {
 public:
  using value_type = T_;
  using shared_handle = std::shared_ptr<value_type>;
  static auto instance() -> shared_handle;
  ~Singleton() = default;

 private:
  Singleton() = default;
  Singleton(Singleton const&) = delete;
  auto operator=(Singleton const&) -> Singleton& = delete;
};

}  // namespace pmt::base

#include "pmt/base/singleton-inl.hpp"