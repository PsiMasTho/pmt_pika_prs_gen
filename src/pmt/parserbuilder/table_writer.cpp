#include "pmt/parserbuilder/table_writer.hpp"

#include "pmt/util/parse/generic_lexer_tables.hpp"

#include <sstream>
#include <utility>

namespace pmt::parserbuilder {
using namespace pmt::util::parse;

namespace {
auto as_hex(std::integral auto value_) -> std::string {
  std::ostringstream os;
  os << "0x" << std::hex << value_;
  return os.str();
}
}  // namespace

TableWriter::TableWriter(std::ostream& os_header_, std::ostream& os_source_, std::string_view class_name_, pmt::util::parse::GenericLexerTables const& tables_)
 : _os_header(os_header_)
 , _os_source(os_source_)
 , _class_name(class_name_)
 , _tables(tables_) {
}

void TableWriter::write() {
  write_header();
  write_source();
}

void TableWriter::write_header() {
  // clang-format off
  _os_header <<
  "class " << _class_name << " {\n"
  " public:\n"
  "  static inline size_t const STATE_COUNT  = " << _tables._transitions.size() << ";\n"
  "  static inline size_t const ACCEPT_COUNT = " << _tables._terminal_names.size() << ";\n"
  "  static inline size_t const ID_COUNT     = " << _tables._id_names.size() << ";\n"
  "\n"
  "  using RawBitsetType = std::array<uint64_t, ACCEPT_COUNT>;\n"
  "\n"
  "  static pmt::util::parse::GenericLexerTables::TransitionType const TRANSITIONS[STATE_COUNT];\n"
  "  static RawBitsetType const                                        ACCEPTS[STATE_COUNT];\n"
  "  static char const* const                                          TERMINAL_NAMES[ACCEPT_COUNT];\n"
  "  static pmt::util::parse::GenericAst::IdType const                 TERMINAL_IDS[ACCEPT_COUNT];\n"
  "  static char const* const                                          ID_NAMES[ID_COUNT];\n"
  "\n"
  "  enum : pmt::util::parse::GenericAst::IdType {\n";
  
  std::string first_eq = " = 0";
  std::string delim;
  for (std::string const& id_name : _tables._id_names) {
    _os_header << std::exchange(delim, ",\n") << "   " << id_name << std::exchange(first_eq, "");
  }

  _os_header <<
  "\n"
  "  };\n"
  "};\n";
  // clang-format on
}

void TableWriter::write_source() {
  _os_source << "pmt::util::parse::GenericLexerTables::TransitionType const " << _class_name << "::TRANSITIONS[" << _class_name << "::STATE_COUNT] = {\n";
  std::string delim1;
  for (size_t i = 0; i < _tables._transitions.size(); ++i) {
    _os_source << std::exchange(delim1, ",\n") << " {";
    std::string delim2;
    for (size_t j = 0; j < _tables._transitions[i].size(); ++j) {
      _os_source << std::exchange(delim2, ", ") << as_hex(_tables._transitions[i][j]);
    }
    _os_source << "}";
  }
  _os_source << "\n};\n"
                "\n";

  delim1.clear();

  _os_source << _class_name << "::RawBitsetType const " << _class_name << "::ACCEPTS[" << _class_name << "::STATE_COUNT] = {\n";

  for (size_t i = 0; i < _tables._accepts.size(); ++i) {
    _os_source << std::exchange(delim1, ",\n") << " {";
    std::string delim2;
    for (size_t j = 0; j < _tables._accepts[i].size(); ++j) {
      _os_source << std::exchange(delim2, ", ") << as_hex(_tables._accepts[i][j]);
    }
    _os_source << "}";
  }

  _os_source << "\n};\n"
                "\n";
  delim1.clear();

  _os_source << "char const* const " << _class_name << "::TERMINAL_NAMES[" << _class_name << "::ACCEPT_COUNT] = {\n";

  for (size_t i = 0; i < _tables._terminal_names.size(); ++i) {
    _os_source << std::exchange(delim1, ",\n") << " \"" << _tables._terminal_names[i] << "\"";
  }

  _os_source << "\n};\n"
                "\n";
  delim1.clear();

  _os_source << "pmt::util::parse::GenericAst::IdType const " << _class_name << "::TERMINAL_IDS[" << _class_name << "::ACCEPT_COUNT] = {\n";

  for (size_t i = 0; i < _tables._terminal_ids.size(); ++i) {
    _os_source << std::exchange(delim1, ",\n") << " " << as_hex(_tables._terminal_ids[i]);
  }

  _os_source << "\n};\n"
                "\n";
  delim1.clear();

  _os_source << "char const* const " << _class_name << "::ID_NAMES[" << _class_name << "::ID_COUNT] = {\n";

  for (size_t i = 0; i < _tables._id_names.size(); ++i) {
    _os_source << std::exchange(delim1, ",\n") << " \"" << _tables._id_names[i] << "\"";
  }

  _os_source << "\n};\n"
                "\n";
  delim1.clear();
}
};  // namespace pmt::parserbuilder
