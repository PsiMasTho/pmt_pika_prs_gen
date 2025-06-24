#include "pmt/parser/rt/lexer_tables_base.hpp"

namespace pmt::parser::rt {
using namespace pmt::base;

auto LexerTablesBase::get_accept_index_display_name(size_t index_) const -> std::string {
 return std::to_string(index_);
}

auto LexerTablesBase::get_accept_index_id(size_t) const -> GenericId::IdType {
 return GenericId::IdUninitialized;
}

}  // namespace pmt::parser::rt