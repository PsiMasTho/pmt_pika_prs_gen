// clang-format off
#ifdef __INTELLISENSE__
    #include "pmt/rt/memo_table.hpp"
#endif
// clang-format on

namespace pmt::rt {

auto MemoTable::KeyEqIndirect::operator()(auto const& lhs_, auto const& rhs_) const -> bool {
 Key const& lhs_key = fetch_key(lhs_);
 Key const& rhs_key = fetch_key(rhs_);
 return lhs_key._clause == rhs_key._clause && lhs_key._position == rhs_key._position;
}

}  // namespace pmt::rt