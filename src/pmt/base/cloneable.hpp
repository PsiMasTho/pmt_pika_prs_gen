#pragma once

#include <memory>
#include <stdexcept>

namespace pmt::base {

class Cloneable {
 public:
  auto clone() const -> std::unique_ptr<Cloneable>;
  virtual ~Cloneable() = default;

 private:
  virtual auto v_clone() const -> std::unique_ptr<Cloneable>;
};

class CloneError : public std::logic_error {
 public:
  CloneError();
};

template <typename CRTP_, typename BASE_>
class InheritWithVClone : public BASE_ {
  static_assert(std::derived_from<BASE_, Cloneable>);

 public:
  using BASE_::BASE_;

 private:
  auto v_clone() const -> std::unique_ptr<Cloneable> override;
};

}  // namespace pmt::base

#include "pmt/base/cloneable-inl.hpp"