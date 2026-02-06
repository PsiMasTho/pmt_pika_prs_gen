#include "pmt/builder/id_emitter.hpp"

#include "pmt/util/timestamp.hpp"
#include "pmt/util/uint_to_str.hpp"

#include <fstream>

namespace pmt::builder {
namespace {}  // namespace

IdEmitter::IdEmitter(Args args_)
 : _args(std::move(args_)) {
}

void IdEmitter::emit() {
 bool const want_strings = _args._strings_output_file != nullptr;
 bool const want_constants = _args._constants_output_file != nullptr;
 if (!want_strings && !want_constants) {
  return;
 }

 std::string const timestamp = pmt::util::get_timestamp();

 if (want_constants) {
  replace_skeleton_label(_args._constants_skel, "TIMESTAMP", timestamp);
 }
 if (want_strings) {
  replace_skeleton_label(_args._strings_skel, "TIMESTAMP", timestamp);
 }

 std::string id_constants;
 std::string id_strings;

 _args._id_table.for_each_id([&](std::string const& id_string_, pmt::rt::IdType id_) {
  if (want_constants) {
   if (!id_constants.empty()) {
    id_constants += "\n";
   }
   id_constants += id_string_;
   id_constants += " = ";
   id_constants += "0x" + pmt::util::uint_to_string(id_, 0, pmt::util::hex_alphabet_uppercase);
   id_constants += ",";
  }

  if (want_strings) {
   if (!id_strings.empty()) {
    id_strings += "\n";
   }
   id_strings += "\"";
   id_strings += id_string_;
   id_strings += "\",";
  }
 });

 if (want_constants) {
  replace_skeleton_label(_args._constants_skel, "ID_CONSTANTS", id_constants);
  *_args._constants_output_file << _args._constants_skel;
 }
 if (want_strings) {
  replace_skeleton_label(_args._strings_skel, "ID_STRINGS", id_strings);
  *_args._strings_output_file << _args._strings_skel;
 }
}

}  // namespace pmt::builder
