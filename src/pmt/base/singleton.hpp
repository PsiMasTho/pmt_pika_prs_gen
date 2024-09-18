#pragma once

#include <concepts>
#include <memory>

#include "pmt/namespace_id.hpp"

namespace pmt::base {

template <std::default_initializable T_, uint64_t ID_ = namespace_id::SHARED_NAMESPACE_ID>
class singleton {
 public:
  using value_type = T_;
  using shared_handle = std::shared_ptr<value_type>;
  static auto instance() -> shared_handle;
  ~singleton() = default;

 private:
  singleton() = default;
  singleton(singleton const&) = delete;
  auto operator=(singleton const&) -> singleton& = delete;
};

}  // namespace pmt::base

#include "pmt/base/singleton-inl.hpp"