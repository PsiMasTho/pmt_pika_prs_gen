#pragma once

#include "pmt/pika/rt/primitives.hpp"

#include <concepts>
#include <string>
#include <unordered_map>
#include <vector>

namespace pmt::pika::meta {

class IdTable {
 std::unordered_map<std::string, pmt::pika::rt::IdType> _id_string_to_value;
 std::vector<std::string> _id_value_to_string;

public:
 void insert_id(std::string const& id_string_);

 auto id_to_string(pmt::pika::rt::IdType id_) const -> std::string;
 auto string_to_id(std::string const& id_string_) const -> pmt::pika::rt::IdType;

 void for_each_id(std::invocable<std::string const&, pmt::pika::rt::IdType> auto&& func_) const;
};

}  // namespace pmt::pika::meta

#include "pmt/pika/meta/id_table-inl.hpp"