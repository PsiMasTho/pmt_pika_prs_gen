#pragma once

#include "pmt/pika/rt/ast.hpp"
#include "pmt/pika/rt/memo_table.hpp"

#include <string_view>

namespace pmt::pika::rt {
class PikaTablesBase;

class PikaParser {
public:
 // -$ Functions $-
 // --$ Other $--
 static auto populate_memo_table(std::string_view input_, PikaTablesBase const& pika_tables_) -> MemoTable;
 static auto memo_table_to_ast(MemoTable const& memo_table_, std::string_view input_, PikaTablesBase const& pika_tables_) -> Ast::UniqueHandle;
};

}  // namespace pmt::pika::rt