#pragma once

#include "pmt/parser/generic_id.hpp"

namespace pmt::parser::rt {

class RuleParametersView {
public:
 static inline bool const MERGE_DEFAULT = false;
 static inline bool const UNPACK_DEFAULT = false;
 static inline bool const HIDE_DEFAULT = false;

 std::string_view _display_name;
 std::string_view _id_string;
 GenericId::IdType _id_value = GenericId::IdUninitialized;
 bool _merge : 1 = MERGE_DEFAULT;
 bool _unpack : 1 = UNPACK_DEFAULT;
 bool _hide : 1 = HIDE_DEFAULT;
};

}  // namespace pmt::parser::rt