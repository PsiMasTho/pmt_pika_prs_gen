// clang-format off
#ifdef __INTELLISENSE__
    #include "pmt/container/multi_vector.hpp"
#endif
// clang-format on

#include <algorithm>
#include <memory>

namespace pmt::container {
template <typename... TS_>
MultiVector<TS_...>::MultiVector()
 : _tuple(std::make_tuple(static_cast<std::remove_pointer_t<TS_>*>(nullptr)...))
 , _size(0)
 , _capacity_idx(0) {
}

template <typename... TS_>
MultiVector<TS_...>::MultiVector(MultiVector const& other_)
 : MultiVector() {
 reserve(other_.size());
 _size = other_.size();  // NOLINT
 [&]<size_t... IDX_>(std::index_sequence<IDX_...>) {
  (..., (std::uninitialized_copy_n(std::get<IDX_>(other_._tuple), other_.size(), std::get<IDX_>(_tuple))));
 }(std::make_index_sequence<sizeof...(TS_)>{});
}

template <typename... TS_>
MultiVector<TS_...>::MultiVector(MultiVector&& other_) noexcept
 : _tuple(other_._tuple)
 , _size(other_._size)
 , _capacity_idx(other_._capacity_idx) {
 other_._tuple = std::make_tuple(static_cast<std::remove_pointer_t<TS_>*>(nullptr)...);
 other_._size = 0;
 other_._capacity_idx = 0;
}

template <typename... TS_>
MultiVector<TS_...>::~MultiVector() {
 clear();
 [&]<size_t... IDX_>(std::index_sequence<IDX_...>) {
  (..., (::operator delete[](std::get<IDX_>(_tuple))));
 }(std::make_index_sequence<sizeof...(TS_)>{});
}

template <typename... TS_>
auto MultiVector<TS_...>::operator=(MultiVector const& other_) -> MultiVector& {
 if (this == &other_) {
  return *this;
 }

 MultiVector tmp(other_);
 std::swap(_tuple, tmp._tuple);

 // swap bitfields manually because std::swap doesnt work
 size_t swap_tmp = _size;
 _size = tmp._size;
 tmp._size = swap_tmp;

 swap_tmp = _capacity_idx;
 _capacity_idx = tmp._capacity_idx;
 tmp._capacity_idx = swap_tmp;

 return *this;
}

template <typename... TS_>
auto MultiVector<TS_...>::operator=(MultiVector&& other_) noexcept -> MultiVector& {
 if (this == &other_) {
  return *this;
 }

 std::swap(_tuple, other_._tuple);

 // swap bitfields manually because std::swap doesnt work
 size_t swap_tmp = _size;
 _size = other_._size;
 other_._size = swap_tmp;

 swap_tmp = _capacity_idx;
 _capacity_idx = other_._capacity_idx;
 other_._capacity_idx = swap_tmp;

 return *this;
}

template <typename... TS_>
auto MultiVector<TS_...>::operator==(MultiVector const& other_) const -> bool {
 return _size == other_._size && ([&]<size_t... IDX_>(std::index_sequence<IDX_...>) {
         return (... && std::equal(std::get<IDX_>(_tuple), std::get<IDX_>(_tuple) + _size, std::get<IDX_>(other_._tuple)));
        }(std::make_index_sequence<sizeof...(TS_)>{}));
}

template <typename... TS_>
auto MultiVector<TS_...>::operator!=(MultiVector const& other_) const -> bool {
 return !(*this == other_);
}

template <typename... TS_>
auto MultiVector<TS_...>::size() const -> size_t {
 return _size;
}

template <typename... TS_>
auto MultiVector<TS_...>::capacity() const -> size_t {
 return ([&]<size_t... IDX_>(std::index_sequence<IDX_...>) {
  return (... || (std::get<IDX_>(_tuple) == nullptr));
 }(std::make_index_sequence<sizeof...(TS_)>{}))
         ? 0
         : AmortizedGrowth::idx_to_size(_capacity_idx);
}

template <typename... TS_>
auto MultiVector<TS_...>::empty() const -> bool {
 return _size == 0;
}

template <typename... TS_>
void MultiVector<TS_...>::clear() {
 [&]<size_t... IDX_>(std::index_sequence<IDX_...>) {
  (..., (std::destroy_n(std::get<IDX_>(_tuple), _size)));
 }(std::make_index_sequence<sizeof...(TS_)>{});
 _size = 0;
}

template <typename... TS_>
void MultiVector<TS_...>::resize(size_t size_, TS_... default_values_) {
 if (size_ > _size) {
  reserve(size_);
  [&]<size_t... IDX_>(std::index_sequence<IDX_...>) {
   (..., (std::uninitialized_fill_n(std::get<IDX_>(_tuple) + _size, size_ - _size, std::get<IDX_>(std::forward_as_tuple(default_values_...)))));
  }(std::make_index_sequence<sizeof...(TS_)>{});
 } else if (size_ < _size) {
  [&]<size_t... IDX_>(std::index_sequence<IDX_...>) {
   (..., (std::destroy_n(std::get<IDX_>(_tuple) + size_, _size - size_)));
  }(std::make_index_sequence<sizeof...(TS_)>{});
 }

 _size = size_;
}

template <typename... TS_>
void MultiVector<TS_...>::reserve(size_t new_capacity_) {
 new_capacity_ = std::max(size(), new_capacity_);

 size_t const new_capacity_idx = AmortizedGrowth::size_to_idx(new_capacity_, InitialCapacityIdx);

 if ([&]<size_t... IDX_>(std::index_sequence<IDX_...>) {
      return (... && (std::get<IDX_>(_tuple) != nullptr));
     }(std::make_index_sequence<sizeof...(TS_)>{}) &&
     new_capacity_idx == _capacity_idx) {
  return;
 }

 size_t const old_capacity = capacity();
 new_capacity_ = AmortizedGrowth::idx_to_size(new_capacity_idx);
 _capacity_idx = new_capacity_idx;

 [&]<size_t... IDX_>(std::index_sequence<IDX_...>) {
  (..., ([&]() {
    using T = std::remove_pointer_t<typename std::tuple_element_t<IDX_, PtrTuple>>;
    T* new_ptr = static_cast<T*>(::operator new[](sizeof(T) * new_capacity_));

    std::uninitialized_move_n(std::get<IDX_>(_tuple), size(), new_ptr);
    std::destroy_n(std::get<IDX_>(_tuple), size());
    ::operator delete[](std::get<IDX_>(_tuple));
    std::get<IDX_>(_tuple) = new_ptr;
   }()));
 }(std::make_index_sequence<sizeof...(TS_)>{});
}

template <typename... TS_>
void MultiVector<TS_...>::push_back(TS_... values_) {
 if (size() == capacity()) {
  reserve(size() + 1);
 }

 [&]<size_t... IDX_>(std::index_sequence<IDX_...>) {
  (..., (std::construct_at(std::get<IDX_>(_tuple) + _size, std::get<IDX_>(std::forward_as_tuple(values_...)))));
 }(std::make_index_sequence<sizeof...(TS_)>{});
 ++_size;
}

template <typename... TS_>
void MultiVector<TS_...>::pop_back() {
 if (_size == 0) {
  return;
 }
 --_size;
 [&]<size_t... IDX_>(std::index_sequence<IDX_...>) {
  (..., (std::destroy_at(std::get<IDX_>(_tuple) + _size)));
 }(std::make_index_sequence<sizeof...(TS_)>{});
}

template <typename... TS_>
void MultiVector<TS_...>::insert(size_t inner_idx_, TS_... values_) {
 reserve(size() + 1);

 [&]<size_t... IDX_>(std::index_sequence<IDX_...>) {
  (..., (std::construct_at(std::get<IDX_>(_tuple) + size(), std::get<IDX_>(std::forward_as_tuple(values_...)))));
 }(std::make_index_sequence<sizeof...(TS_)>{});
 ++_size;

 [&]<size_t... IDX_>(std::index_sequence<IDX_...>) {
  (..., (std::rotate(std::get<IDX_>(_tuple) + inner_idx_, std::get<IDX_>(_tuple) + size() - 1, std::get<IDX_>(_tuple) + size())));
 }(std::make_index_sequence<sizeof...(TS_)>{});
}

template <typename... TS_>
void MultiVector<TS_...>::erase(size_t inner_idx_) {
 [&]<size_t... IDX_>(std::index_sequence<IDX_...>) {
  (..., (std::move(std::get<IDX_>(_tuple) + inner_idx_ + 1, std::get<IDX_>(_tuple) + _size, std::get<IDX_>(_tuple) + inner_idx_)));
 }(std::make_index_sequence<sizeof...(TS_)>{});

 [&]<size_t... IDX_>(std::index_sequence<IDX_...>) {
  (..., (std::destroy_at(std::get<IDX_>(_tuple) + _size - 1)));
 }(std::make_index_sequence<sizeof...(TS_)>{});

 --_size;
}

template <typename... TS_>
void MultiVector<TS_...>::swap_elements(size_t inner_idx_a_, size_t inner_idx_b_) {
 [&]<size_t... IDX_>(std::index_sequence<IDX_...>) {
  (..., (std::swap(*(std::get<IDX_>(_tuple) + inner_idx_a_), *(std::get<IDX_>(_tuple) + inner_idx_b_))));
 }(std::make_index_sequence<sizeof...(TS_)>{});
}

template <typename... TS_>
template <size_t OUTER_IDX_>
auto MultiVector<TS_...>::get_span_by_index() -> std::span<std::remove_pointer_t<typename std::tuple_element_t<OUTER_IDX_, PtrTuple>>> {
 return std::span<std::remove_pointer_t<typename std::tuple_element_t<OUTER_IDX_, PtrTuple>>>(std::get<OUTER_IDX_>(_tuple), size());
}

template <typename... TS_>
template <size_t OUTER_IDX_>
auto MultiVector<TS_...>::get_span_by_index() const -> std::span<std::remove_pointer_t<typename std::tuple_element_t<OUTER_IDX_, PtrTuple>> const> {
 return std::span<std::remove_pointer_t<typename std::tuple_element_t<OUTER_IDX_, PtrTuple>> const>(std::get<OUTER_IDX_>(_tuple), size());
}

template <typename... TS_>
template <typename T_>
auto MultiVector<TS_...>::get_span_by_type() -> std::span<T_> {
 return std::span<T_>(std::get<T_*>(_tuple), size());
}

template <typename... TS_>
template <typename T_>
auto MultiVector<TS_...>::get_span_by_type() const -> std::span<T_ const> {
 return std::span<T_ const>(std::get<T_*>(_tuple), size());
}

template <typename... TS_>
auto MultiVector<TS_...>::get_tuple(size_t inner_idx_) -> ValueReferenceTuple {
 return [&]<size_t... IDX_>(std::index_sequence<IDX_...>) {
  return ValueReferenceTuple(*(std::get<IDX_>(_tuple) + inner_idx_)...);
 }(std::make_index_sequence<sizeof...(TS_)>{});
}

template <typename... TS_>
auto MultiVector<TS_...>::get_tuple(size_t inner_idx_) const -> ValueConstReferenceTuple {
 return [&]<size_t... IDX_>(std::index_sequence<IDX_...>) {
  return ValueConstReferenceTuple(*(std::get<IDX_>(_tuple) + inner_idx_)...);
 }(std::make_index_sequence<sizeof...(TS_)>{});
}

}  // namespace pmt::container
