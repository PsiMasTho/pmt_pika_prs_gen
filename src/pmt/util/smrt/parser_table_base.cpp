#include "pmt/util/smrt/parser_tables_base.hpp"

namespace pmt::util::smrt {

auto ParserTablesBase::get_nonterminal_label(size_t index_) const -> std::string {
 return std::to_string(index_);

}

auto ParserTablesBase::get_nonterminal_id(size_t) const -> GenericId::IdType {
 return GenericId::IdUninitialized;
}

auto ParserTablesBase::id_to_string(GenericId::IdType id_) const -> std::string {
 return std::to_string(id_);
}


}