#pragma once

#include "pmt/rt/ast.hpp"
#include "pmt/rt/memo_table.hpp"

#include <string_view>

namespace pmt::rt {
class PikaProgramBase;

class PikaParser {
public:
 // -$ Types / Constants $-
 // -$ Functions $-
 // --$ Other $--
 static auto populate_memo_table(std::string_view input_, PikaProgramBase const& pika_program_) -> MemoTable;
 static auto memo_table_to_ast(MemoTable const& memo_table_, std::string_view input_, PikaProgramBase const& pika_program_) -> Ast::UniqueHandle;
};

}  // namespace pmt::rt