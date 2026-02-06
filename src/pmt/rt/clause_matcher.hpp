#pragma once

#include "pmt/rt/memo_table.hpp"

#include <string_view>

namespace pmt::rt {

class ClauseMatcher {
public:
 static auto match(MemoTable const& memo_table_, MemoTable::Key key_, std::string_view input_, PikaTablesBase const& pika_tables_) -> std::optional<MemoTable::Match>;
};

}  // namespace pmt::rt