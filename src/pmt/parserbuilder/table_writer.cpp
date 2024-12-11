#include "pmt/parserbuilder/table_writer.hpp"

#include "pmt/parserbuilder/lexer_tables.hpp"

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

TableWriter::TableWriter(std::ostream& os_header_, std::ostream& os_source_, std::string_view class_name_, LexerTables const& tables_)
 : _os_header(os_header_)
 , _os_source(os_source_)
 , _tables(tables_)
 , _class_name(class_name_) {
}

void TableWriter::write() {
  write_header();
  write_source();
}

void TableWriter::write_header() {
  // clang-format off

  _os_header << "\n"
  "  auto id_to_string(pmt::util::parse::GenericAst::IdType id_) -> std::string_view;\n"
  "  auto as_generic_lexer_tables() const -> pmt::util::parsert::GenericLexerTables;\n";
  // clang-format on
}

void TableWriter::write_source() {
  static size_t const PER_LINE = 16;
  _os_source << "uint64_t const " << _class_name << "::TRANSITIONS_DEFAULT[" << _class_name << "::DEFAULT_COUNT] = {\n";
  for (size_t i = 0; i < _tables._transitions_default.size(); ++i) {
    if (i == 0) {
      _os_source << " ";
    } else if (i % PER_LINE == 0) {
      _os_source << ",\n ";
    } else {
      _os_source << ", ";
    }
    _os_source << as_hex(_tables._transitions_default[i]);
  }
  _os_source << "\n};\n"
                "\n";

  _os_source << "uint64_t const " << _class_name << "::TRANSITIONS_SHIFTS[" << _class_name << "::SHIFT_COUNT] = {\n";
  for (size_t i = 0; i < _tables._transitions_shift.size(); ++i) {
    if (i == 0) {
      _os_source << " ";
    } else if (i % PER_LINE == 0) {
      _os_source << ",\n ";
    } else {
      _os_source << ", ";
    }
    _os_source << as_hex(_tables._transitions_shift[i]);
  }
  _os_source << "\n};\n"
                "\n";

  _os_source << "uint64_t const " << _class_name << "::TRANSITIONS_NEXT[" << _class_name << "::NEXT_COUNT] = {\n";
  for (size_t i = 0; i < _tables._transitions_next.size(); ++i) {
    if (i == 0) {
      _os_source << " ";
    } else if (i % PER_LINE == 0) {
      _os_source << ",\n ";
    } else {
      _os_source << ", ";
    }
    _os_source << as_hex(_tables._transitions_next[i]);
  }
  _os_source << "\n};\n"
                "\n";

  _os_source << "uint64_t const " << _class_name << "::TRANSITIONS_CHECK[" << _class_name << "::CHECK_COUNT] = {\n";
  for (size_t i = 0; i < _tables._transitions_check.size(); ++i) {
    if (i == 0) {
      _os_source << " ";
    } else if (i % PER_LINE == 0) {
      _os_source << ",\n ";
    } else {
      _os_source << ", ";
    }
    _os_source << as_hex(_tables._transitions_check[i]);
  }
  _os_source << "\n};\n"
                "\n";

  std::string delim1;
  _os_source << "uint64_t const " << _class_name << "::ACCEPTS[" << _class_name << "::STATE_COUNT] = {\n";

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
    if (i == 0) {
      _os_source << " ";
    } else if (i % PER_LINE == 0) {
      _os_source << ",\n ";
    } else {
      _os_source << ", ";
    }
    _os_source << as_hex(_tables._terminal_ids[i]);
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

void TableWriter::write_header_id_enum() {
  _os_header << "class " << _class_name
             << " {\n"
                " public:\n"
                "  enum : pmt::util::parse::GenericAst::IdType {\n";

  std::string first_eq = " = 0";
  std::string delim;
  for (std::string const& id_name : _tables._id_names) {
    _os_header << std::exchange(delim, ",\n") << "   " << id_name << std::exchange(first_eq, "");
  }

  _os_header << "\n"
                "  };\n"
                "};\n";
}

};  // namespace pmt::parserbuilder
