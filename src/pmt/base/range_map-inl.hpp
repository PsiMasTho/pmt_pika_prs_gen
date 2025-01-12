// clang-format off
#ifdef __INTELLISENSE__
    #include "pmt/base/range_map.hpp"
#endif
// clang-format on

namespace pmt::base {

template <typename KEY_, typename VALUE_, typename KEY_CMP_, typename VALUE_EQ_>
template <typename KEY_T_1_, typename KEY_T_2_, typename VALUE_T_>
void RangeMap<KEY_, VALUE_, KEY_CMP_, VALUE_EQ_>::insert(KEY_T_1_ lower_, KEY_T_2_ upper_, VALUE_T_ value_) {
}

template <typename KEY_, typename VALUE_, typename KEY_CMP_, typename VALUE_EQ_>
template <typename KEY_T_, typename VALUE_T_>
void RangeMap<KEY_, VALUE_, KEY_CMP_, VALUE_EQ_>::insert(KEY_T_ key_, VALUE_T_ value_) {
}

template <typename KEY_, typename VALUE_, typename KEY_CMP_, typename VALUE_EQ_>
template <typename KEY_T_1_, typename KEY_T_2_, typename... ARGS_>
void RangeMap<KEY_, VALUE_, KEY_CMP_, VALUE_EQ_>::emplace(KEY_T_1_ lower_, KEY_T_2_ upper_, ARGS_&&... args_) {
}

template <typename KEY_, typename VALUE_, typename KEY_CMP_, typename VALUE_EQ_>
template <typename KEY_T_, typename... ARGS_>
void RangeMap<KEY_, VALUE_, KEY_CMP_, VALUE_EQ_>::emplace(KEY_T_ key_, ARGS_&&... args_) {
}

template <typename KEY_, typename VALUE_, typename KEY_CMP_, typename VALUE_EQ_>
void RangeMap<KEY_, VALUE_, KEY_CMP_, VALUE_EQ_>::erase(KEY_ const& lower_, KEY_ const& upper_) {
}

template <typename KEY_, typename VALUE_, typename KEY_CMP_, typename VALUE_EQ_>
void RangeMap<KEY_, VALUE_, KEY_CMP_, VALUE_EQ_>::erase(KEY_ const& key_) {
}

template <typename KEY_, typename VALUE_, typename KEY_CMP_, typename VALUE_EQ_>
void RangeMap<KEY_, VALUE_, KEY_CMP_, VALUE_EQ_>::clear() {
  _lowers.clear();
  _uppers.clear();
  _values.clear();
}

template <typename KEY_, typename VALUE_, typename KEY_CMP_, typename VALUE_EQ_>
auto RangeMap<KEY_, VALUE_, KEY_CMP_, VALUE_EQ_>::find(KEY_ const& key_) const -> VALUE_ const* {
}

template <typename KEY_, typename VALUE_, typename KEY_CMP_, typename VALUE_EQ_>
auto RangeMap<KEY_, VALUE_, KEY_CMP_, VALUE_EQ_>::get(size_t index_) -> std::tuple<KEY_ const&, KEY_ const&, VALUE_ const&> {
  return {_lowers[index_], _uppers[index_], _values[index_]};
}

template <typename KEY_, typename VALUE_, typename KEY_CMP_, typename VALUE_EQ_>
auto RangeMap<KEY_, VALUE_, KEY_CMP_, VALUE_EQ_>::size() const -> size_t {
  return _values.size();
}

template <typename KEY_, typename VALUE_, typename KEY_CMP_, typename VALUE_EQ_>
auto RangeMap<KEY_, VALUE_, KEY_CMP_, VALUE_EQ_>::empty() const -> bool {
  return _values.empty();
}

template <typename KEY_, typename VALUE_, typename KEY_CMP_, typename VALUE_EQ_>
void RangeMap<KEY_, VALUE_, KEY_CMP_, VALUE_EQ_>::reserve(size_t size_) {
  _lowers.reserve(size_);
  _uppers.reserve(size_);
  _values.reserve(size_);
}

}  // namespace pmt::base