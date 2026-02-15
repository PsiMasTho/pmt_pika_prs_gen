// clang-format off
#ifdef __INTELLISENSE__
    #include "pmt/pika/meta/id_table.hpp"
#endif
// clang-format on

namespace pmt::pika::meta {

void IdTable::for_each_id(std::invocable<std::string const&, pmt::pika::rt::IdType> auto&& func_) const {
 for (pmt::pika::rt::IdType id = 0; std::string const& id_string : _id_value_to_string) {
  std::forward<decltype(func_)>(func_)(id_string, id++);
 }
}

}  // namespace pmt::pika::meta