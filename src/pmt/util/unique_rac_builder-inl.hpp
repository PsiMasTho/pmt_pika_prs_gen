// clang-format off
#ifdef __INTELLISENSE__
    #include "pmt/util/unique_rac_builder.hpp"
#endif
// clang-format on

#include <cassert>

namespace pmt::util {

template <typename CONTAINER_T_>
auto unique_rac_builder_fetch(CONTAINER_T_ const& container_, size_t idx_) -> typename CONTAINER_T_::value_type const& {
 assert(idx_ < container_.size());
 return container_[idx_];
}

template <typename CONTAINER_T_>
auto unique_rac_builder_fetch(CONTAINER_T_ const& container_, typename CONTAINER_T_::value_type const& item_) -> typename CONTAINER_T_::value_type const& {
 return item_;
}

template <typename CONTAINER_T_, typename HASH_T_>
UniqueRacBuilderHasher<CONTAINER_T_, HASH_T_>::UniqueRacBuilderHasher(CONTAINER_T_ const& container_)
 : _container(container_) {
}

template <typename CONTAINER_T_, typename HASH_T_>
auto UniqueRacBuilderHasher<CONTAINER_T_, HASH_T_>::operator()(auto const& item_) const -> size_t {
 return HASH_T_{}(unique_rac_builder_fetch(_container, item_));
}

template <typename CONTAINER_T_, typename EQ_T_>
UniqueRacBuilderEq<CONTAINER_T_, EQ_T_>::UniqueRacBuilderEq(CONTAINER_T_ const& container_)
 : _container(container_) {
}

template <typename CONTAINER_T_, typename EQ_T_>
auto UniqueRacBuilderEq<CONTAINER_T_, EQ_T_>::operator()(auto const& lhs_, auto const& rhs_) const -> bool {
 return EQ_T_{}(unique_rac_builder_fetch(_container, lhs_), unique_rac_builder_fetch(_container, rhs_));
}

template <typename CONTAINER_T_, typename HASH_T_, typename EQ_T_>
UniqueRacBuilder<CONTAINER_T_, HASH_T_, EQ_T_>::UniqueRacBuilder(CONTAINER_T_& container_)
 : _index_cache(0, UniqueRacBuilderHasher<CONTAINER_T_, HASH_T_>(container_), UniqueRacBuilderEq<CONTAINER_T_, EQ_T_>(container_))
 , _container(container_) {
  assert(container_.empty());
}

template <typename CONTAINER_T_, typename HASH_T_, typename EQ_T_>
auto UniqueRacBuilder<CONTAINER_T_, HASH_T_, EQ_T_>::insert_and_get_index(typename CONTAINER_T_::value_type item_) -> std::pair<size_t, bool> {
 if (auto const itr = _index_cache.find(item_); itr != _index_cache.end()) {
  return {*itr, false};
 }

 _container.push_back(std::move(item_));
 _index_cache.insert(_container.size() - 1);

 return {_container.size() - 1, true};
}

template <typename CONTAINER_T_, typename HASH_T_, typename EQ_T_>
auto UniqueRacBuilder<CONTAINER_T_, HASH_T_, EQ_T_>::insert(typename CONTAINER_T_::value_type item_) -> bool {
 if (auto const itr = _index_cache.find(item_); itr != _index_cache.end()) {
  return false;
 }

 _container.push_back(std::move(item_));
 _index_cache.insert(_container.size() - 1);
 return true;
}

template <typename CONTAINER_T_, typename HASH_T_, typename EQ_T_>
auto UniqueRacBuilder<CONTAINER_T_, HASH_T_, EQ_T_>::get_container() -> CONTAINER_T_& {
 return _container;
}

}  // namespace pmt::util