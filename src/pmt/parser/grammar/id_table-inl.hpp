// clang-format off
#ifdef __INTELLISENSE__
    #include "pmt/parser/grammar/id_table.hpp"
#endif
// clang-format on

namespace pmt::parser::grammar {

void IdTable::for_each_id(std::invocable<std::string const&, GenericId::IdType> auto&& func_) const {
 for (GenericId::IdType id = 0; std::string const& id_string : _id_value_to_string) {
  std::forward<decltype(func_)>(func_)(id_string, id++);
 }
}

}  // namespace pmt::parser::grammar