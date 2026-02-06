#pragma once

#include "pmt/rt/ast.hpp"

namespace pmt::meta {

class Ids {
public:
 enum : pmt::rt::IdType {
#include "pmt/meta/id_constants-inl.hpp"
 };

 static auto id_to_string(pmt::rt::IdType id_) -> std::string;
};

}  // namespace pmt::meta