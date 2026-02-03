#pragma once

#include "pmt/ast/id.hpp"

namespace pmt::meta {

class Ids {
public:
 enum : pmt::ast::IdType {
#include "pmt/meta/id_constants-inl.hpp"
 };

 static auto id_to_string(pmt::ast::IdType id_) -> std::string;
};

}  // namespace pmt::meta