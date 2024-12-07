#include "pmt/parserbuilder/table_writer.hpp"

#include "pmt/util/parse/generic_lexer_tables.hpp"

namespace pmt::parserbuilder {
using namespace pmt::util::parse;

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
    "  using TransitionType = std::array<pmt::util::parse::Fa::StateNrType, UCHAR_MAX>;\n"
    "  using AcceptType     = pmt::base::DynamicBitset;\n"
    "\n"
    "  static inline size_t const STATE_COUNT = " << _tables._transitions.size() << ";\n"
    "  static inline size_t const ACCEPT_COUNT = " << _tables._terminal_names.size() << ";\n"
    "\n"
    "  static TransitionType const                       TRANSITIONS[STATE_COUNT];\n"
    "  static AcceptType const                           ACCEPTS[STATE_COUNT];\n"
    "  static char const* const                          TERMINAL_NAMES[ACCEPT_COUNT];\n"
    "  static pmt::util::parse::GenericAst::IdType const TERMINAL_IDS[ACCEPT_COUNT];\n"
    "  static char const* const                          ID_NAMES[ACCEPT_COUNT];\n"
    "\n"
    "  enum : pmt::util::parse::GenericAst::IdType {\n";
    
    for (size_t i = 0; i < _tables._id_names.size(); ++i) {
      _os_header << "   " << _tables._id_names[i] << " = " << i << ",\n";
    }

    _os_header <<
    "  };\n"
    "};\n";
  // clang-format on
}

void TableWriter::write_source() {
  // clang-format off
  // clang-format on
}
};  // namespace pmt::parserbuilder
