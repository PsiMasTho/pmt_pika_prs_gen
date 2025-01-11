#pragma once

#include <memory>
#include <stdexcept>

namespace pmt::base {

template <typename T_>
class Cloneable {
 public:
  virtual ~Cloneable() = default;

  virtual auto clone() const -> std::unique_ptr<T_>;

 protected:
  Cloneable() = default;
};

class CloneError : public std::logic_error {
 public:
  CloneError();
};

template <typename CRTP_, typename BASE_>
class InheritWithClone : public BASE_ {
  static_assert(std::derived_from<BASE_, Cloneable<BASE_>>);

 public:
  using BASE_::BASE_;

  auto clone() const -> std::unique_ptr<BASE_> override;
};

template <typename CRTP_, typename BASE_>
class VirtualInheritWithClone : virtual public BASE_ {
  static_assert(std::derived_from<BASE_, Cloneable<BASE_>>);

 public:
  using BASE_::BASE_;

  auto clone() const -> std::unique_ptr<BASE_> override;
};

}  // namespace pmt::base

#include "pmt/base/cloneable-inl.hpp"