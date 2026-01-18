#pragma once

#include "pmt/rt/memo_table.hpp"

namespace pmt::rt {

class ClauseMatcher {
public:
 static auto match(MemoTable const& memo_table_, MemoTable::Key key_, std::string_view input_) -> std::optional<MemoTable::Match>;
};

}  // namespace pmt::rt