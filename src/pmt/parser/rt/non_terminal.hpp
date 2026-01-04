#pragma once

#include "pmt/parser/generic_id.hpp"

namespace pmt::parser::rt {

class NonTerminal {
public:
 static inline bool const MERGE_DEFAULT = false;
 static inline bool const UNPACK_DEFAULT = false;
 static inline bool const HIDE_DEFAULT = false;

 std::string _display_name;
 GenericId::IdType _rule_id = GenericId::IdUninitialized;
 bool _merge : 1 = MERGE_DEFAULT;
 bool _unpack : 1 = UNPACK_DEFAULT;
 bool _hide : 1 = HIDE_DEFAULT;

 auto operator==(NonTerminal const& other_) const -> bool = default;
};

}  // namespace pmt::parser::rt