#pragma once

#include "pmt/base/amortized_growth.hpp"

#include <cstddef>
#include <span>
#include <tuple>
#include <type_traits>

namespace pmt::base {

template <typename... TS_>
class MultiVector {
 // -$ Types / Constants $-
 using PtrTuple = std::tuple<TS_*...>;

 enum : size_t {
  InitialCapacityIdx = 3
 };

public:
 using ValueReferenceTuple = std::tuple<std::remove_pointer_t<TS_>&...>;
 using ValueConstReferenceTuple = std::tuple<std::remove_pointer_t<TS_> const&...>;

private:
 // -$ Data $-
 PtrTuple _tuple;
 size_t _size : AmortizedGrowth::MaxCapacityBitWidth;
 size_t _capacity_idx : AmortizedGrowth::MaxCapacityIdxBitWidth;

public:
 // -$ Functions $-
 // --$ Lifetime $--
 MultiVector();
 MultiVector(MultiVector const& other_);
 MultiVector(MultiVector&& other_) noexcept;
 ~MultiVector();

 // --$ Operators $--
 auto operator=(MultiVector const& other_) -> MultiVector&;
 auto operator=(MultiVector&& other_) noexcept -> MultiVector&;
 auto operator==(MultiVector const& other_) const -> bool;
 auto operator!=(MultiVector const& other_) const -> bool;

 // -- $ Other $ --
 auto size() const -> size_t;
 auto capacity() const -> size_t;
 auto empty() const -> bool;
 void clear();
 void resize(size_t size_, TS_... default_values_);
 void reserve(size_t new_capacity_);
 void push_back(TS_... values_);
 void pop_back();
 void insert(size_t inner_idx_, TS_... values_);
 void erase(size_t inner_idx_);
 void swap_elements(size_t inner_idx_a_, size_t inner_idx_b_);

 template <std::size_t OUTER_IDX_>
 auto get_span_by_index() -> std::span<std::remove_pointer_t<typename std::tuple_element_t<OUTER_IDX_, PtrTuple>>>;

 template <std::size_t OUTER_IDX_>
 auto get_span_by_index() const -> std::span<std::remove_pointer_t<typename std::tuple_element_t<OUTER_IDX_, PtrTuple>> const>;

 template <typename T_>
 auto get_span_by_type() -> std::span<T_>;

 template <typename T_>
 auto get_span_by_type() const -> std::span<T_ const>;

 auto get_tuple(size_t inner_idx_) -> ValueReferenceTuple;

 auto get_tuple(size_t inner_idx_) const -> ValueConstReferenceTuple;
};

}  // namespace pmt::base

#include "pmt/base/multi_vector-inl.hpp"
