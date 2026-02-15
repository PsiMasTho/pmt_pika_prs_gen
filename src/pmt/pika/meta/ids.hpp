#pragma once

#include "pmt/pika/rt/ast.hpp"

namespace pmt::pika::meta {

class Ids {
public:
 enum : pmt::pika::rt::IdType {
#include "pmt/pika/meta/id_constants-inl.hpp"
 };

 static auto id_to_string(pmt::pika::rt::IdType id_) -> std::string;
};

}  // namespace pmt::pika::meta