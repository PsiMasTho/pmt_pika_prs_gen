#include "pmt/builder/id_table_to_cpp_str.hpp"

#include "pmt/meta/id_table.hpp"
#include "pmt/util/get_timestamp_str.hpp"
#include "pmt/util/singleton.hpp"
#include "pmt/util/skeleton_replacer.hpp"
#include "pmt/util/uint_to_str.hpp"

#include <utility>

namespace pmt::builder {
using namespace pmt::rt;
using namespace pmt::meta;
using namespace pmt::util;
namespace {

auto entry_to_str_id_constants(std::string const& id_string_, IdType id_constant_) -> std::string {
 return id_string_ + " = " + "0x" + uint_to_string(id_constant_, 0, hex_alphabet_uppercase);
}

auto entry_to_str_id_strings(std::string const& id_string_, IdType) -> std::string {
 return "\"" + id_string_ + "\"";
}

using EntryToStrFnType = std::string (*)(std::string const&, IdType);

auto entries_to_str(IdTable const& id_table_, EntryToStrFnType body_fn_) -> std::string {
 char const* delim = "";
 std::string ret;
 id_table_.for_each_id([&](std::string const& id_string_, IdType id_constant_) { ret.append(std::exchange(delim, "\n") + body_fn_(id_string_, id_constant_) + ","); });
 return ret;
}

auto populate_skel(IdTable const& id_table_, std::string skel_, std::string_view label_, EntryToStrFnType body_fn_) -> std::string {
 auto skeleton_replacer = Singleton<SkeletonReplacer>::instance();
 skeleton_replacer->replace_label(skel_, "TIMESTAMP", get_timestamp_str());
 skeleton_replacer->replace_label(skel_, label_, entries_to_str(id_table_, body_fn_));
 return skel_;
}

}  // namespace

auto id_table_constants_to_cpp_str(IdTable const& id_table_, std::string id_constants_skel_) -> std::string {
 return populate_skel(id_table_, std::move(id_constants_skel_), "ID_CONSTANTS", entry_to_str_id_constants);
}

auto id_table_strings_to_cpp_str(IdTable const& id_table_, std::string id_strings_skel_) -> std::string {
 return populate_skel(id_table_, std::move(id_strings_skel_), "ID_STRINGS", entry_to_str_id_strings);
}

}  // namespace pmt::builder
