#pragma once

#include <cstddef>
#include <type_traits>
#include <unordered_set>

namespace pmt::util {

template <typename CONTAINER_T_>
auto unique_rac_builder_fetch(CONTAINER_T_ const& container_, size_t idx_) -> typename CONTAINER_T_::value_type const&;

template <typename CONTAINER_T_>
auto unique_rac_builder_fetch(CONTAINER_T_ const& container_, typename CONTAINER_T_::value_type const& item_) -> typename CONTAINER_T_::value_type const&;

template <typename CONTAINER_T_, typename HASH_T_>
class UniqueRacBuilderHasher {
 CONTAINER_T_ const& _container;

public:
 using is_transparent = void;  // NOLINT

 explicit UniqueRacBuilderHasher(CONTAINER_T_ const& container_);

 auto operator()(auto const& item_) const -> size_t;
};

template <typename CONTAINER_T_, typename EQ_T_>
class UniqueRacBuilderEq {
 CONTAINER_T_ const& _container;

public:
 using is_transparent = void;  // NOLINT

 explicit UniqueRacBuilderEq(CONTAINER_T_ const& container_);

 auto operator()(auto const& lhs_, auto const& rhs_) const -> bool;
};

// Unique Random Access Container Builder
template <typename CONTAINER_T_, typename HASH_T_ = std::hash<typename CONTAINER_T_::value_type>, typename EQ_T_ = std::equal_to<typename CONTAINER_T_::value_type>>
class UniqueRacBuilder {
 static_assert(!std::is_same_v<size_t, typename CONTAINER_T_::value_type>, "The container value type cannot be size_t");

 // -$ Data $-
 std::unordered_set<size_t, UniqueRacBuilderHasher<CONTAINER_T_, HASH_T_>, UniqueRacBuilderEq<CONTAINER_T_, EQ_T_>> _index_cache;
 CONTAINER_T_& _container;

public:
 // -$ Functions $-
 // --$ Lifetime $--
 explicit UniqueRacBuilder(CONTAINER_T_& container_);

 // --$ Other $--
 auto insert_and_get_index(typename CONTAINER_T_::value_type item_) -> std::pair<size_t, bool>;
 auto insert(typename CONTAINER_T_::value_type item_) -> bool;

 auto get_container() -> CONTAINER_T_&;
};

}  // namespace pmt::util

#include "pmt/util/unique_rac_builder-inl.hpp"