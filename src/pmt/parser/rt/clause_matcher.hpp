#pragma once

#include "pmt/parser/rt/memo_table.hpp"

namespace pmt::parser::rt {

class ClauseMatcher {
public:
 static auto match(MemoTable const& memo_table_, MemoTable::Key key_, std::string_view input_) -> std::optional<MemoTable::Match>;
};

}  // namespace pmt::parser::rt