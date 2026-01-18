#pragma once

#include "pmt/rt/ast_id.hpp"

namespace pmt::meta {

class Language {
public:
 enum : pmt::rt::AstId::IdType {
#include "pmt/meta/id_constants-inl.hpp"
 };

 static auto id_to_string(pmt::rt::AstId::IdType id_) -> std::string;
};

}  // namespace pmt::meta