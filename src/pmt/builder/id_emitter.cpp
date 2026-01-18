#include "pmt/builder/id_emitter.hpp"

#include "pmt/builder/emitter_utils.hpp"
#include "pmt/util/timestamp.hpp"

#include <fstream>

namespace pmt::builder {
using namespace pmt::rt;
namespace {}  // namespace

IdEmitter::IdEmitter(Args args_)
 : _args(std::move(args_)) {
}

void IdEmitter::emit() {
 std::string output_constants = read_stream(_args._constants_skel_file, "ID constants skeleton");
 std::string output_strings = read_stream(_args._strings_skel_file, "ID strings skeleton");

 std::string const timestamp = pmt::util::get_timestamp();

 replace_skeleton_label(output_constants, "TIMESTAMP", timestamp);
 replace_skeleton_label(output_strings, "TIMESTAMP", timestamp);

 std::string id_constants;
 std::string id_strings;

 _args._id_table.for_each_id([&](std::string const& id_string_, AstId::IdType id_) {
  if (!id_constants.empty()) {
   id_constants += "\n";
  }
  id_constants += id_string_;
  id_constants += " = ";
  id_constants += format_hex(id_);
  id_constants += ",";

  if (!id_strings.empty()) {
   id_strings += "\n";
  }
  id_strings += "\"";
  id_strings += id_string_;
  id_strings += "\",";
 });

 replace_skeleton_label(output_constants, "ID_CONSTANTS", id_constants);
 replace_skeleton_label(output_strings, "ID_STRINGS", id_strings);

 _args._strings_output_file << output_strings;
 _args._constants_output_file << output_constants;
}

}  // namespace pmt::builder
