// clang-format off
#ifdef __INTELLISENSE__
    #include "pmt/meta/id_table.hpp"
#endif
// clang-format on

namespace pmt::meta {

void IdTable::for_each_id(std::invocable<std::string const&, pmt::rt::IdType> auto&& func_) const {
 for (pmt::rt::IdType id = 0; std::string const& id_string : _id_value_to_string) {
  std::forward<decltype(func_)>(func_)(id_string, id++);
 }
}

}  // namespace pmt::meta