#include "pmt/builder/id_emitter.hpp"

#include "pmt/util/timestamp.hpp"
#include "pmt/util/uint_to_str.hpp"

#include <fstream>

namespace pmt::builder {
using namespace pmt::rt;
namespace {}  // namespace

IdEmitter::IdEmitter(Args args_)
 : _args(std::move(args_)) {
}

void IdEmitter::emit() {
 std::string const timestamp = pmt::util::get_timestamp();

 replace_skeleton_label(_args._constants_skel, "TIMESTAMP", timestamp);
 replace_skeleton_label(_args._strings_skel, "TIMESTAMP", timestamp);

 std::string id_constants;
 std::string id_strings;

 _args._id_table.for_each_id([&](std::string const& id_string_, AstId::IdType id_) {
  if (!id_constants.empty()) {
   id_constants += "\n";
  }
  id_constants += id_string_;
  id_constants += " = ";
  id_constants += "0x" + pmt::util::uint_to_string(id_, 0, pmt::util::hex_alphabet_uppercase);
  id_constants += ",";

  if (!id_strings.empty()) {
   id_strings += "\n";
  }
  id_strings += "\"";
  id_strings += id_string_;
  id_strings += "\",";
 });

 replace_skeleton_label(_args._constants_skel, "ID_CONSTANTS", id_constants);
 replace_skeleton_label(_args._strings_skel, "ID_STRINGS", id_strings);

 _args._strings_output_file << _args._strings_skel;
 _args._constants_output_file << _args._constants_skel;
}

}  // namespace pmt::builder
