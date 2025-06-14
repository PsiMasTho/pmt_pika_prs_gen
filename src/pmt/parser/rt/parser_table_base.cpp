#include "pmt/parser/rt/parser_tables_base.hpp"

namespace pmt::parser::rt {

auto ParserTablesBase::get_accept_index_label(size_t index_) const -> std::string {
  return std::to_string(index_);
}

auto ParserTablesBase::get_accept_index_unpack(size_t index_) const -> bool {
  return false;
}

auto ParserTablesBase::get_accept_index_hide(size_t index_) const -> bool {
  return false;
}

auto ParserTablesBase::get_accept_index_merge(size_t index_) const -> bool {
  return false;
}

auto ParserTablesBase::get_accept_index_id(size_t) const -> GenericId::IdType {
  return GenericId::IdUninitialized;
}

}  // namespace pmt::parser::rt