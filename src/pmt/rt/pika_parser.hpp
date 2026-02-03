#pragma once

#include "pmt/ast/ast.hpp"
#include "pmt/rt/memo_table.hpp"

#include <string_view>

namespace pmt::rt {
class PikaTablesBase;

class PikaParser {
public:
 // -$ Types / Constants $-
 // -$ Functions $-
 // --$ Other $--
 static auto populate_memo_table(std::string_view input_, PikaTablesBase const& pika_tables_) -> MemoTable;
 static auto memo_table_to_ast(MemoTable const& memo_table_, std::string_view input_, PikaTablesBase const& pika_tables_) -> pmt::ast::Ast::UniqueHandle;
};

}  // namespace pmt::rt