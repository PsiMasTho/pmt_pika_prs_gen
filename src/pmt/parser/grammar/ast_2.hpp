#pragma once

#include "pmt/parser/generic_id.hpp"

namespace pmt::parser::grammar {

class Ast2 {
public:
 enum : GenericId::IdType {
#include "pmt/parser/grammar/id_constants_2-inl.hpp"
 };

 static auto id_to_string(GenericId::IdType id_) -> std::string;
};

}  // namespace pmt::parser::grammar