#pragma once

#include "pmt/rt/ast_id.hpp"

#include <concepts>
#include <string>
#include <unordered_map>
#include <vector>

namespace pmt::meta {

class IdTable {
 std::unordered_map<std::string, pmt::rt::AstId::IdType> _id_string_to_value;
 std::vector<std::string> _id_value_to_string;

public:
 void insert_id(std::string const& id_string_);

 auto id_to_string(pmt::rt::AstId::IdType id_) const -> std::string;
 auto string_to_id(std::string const& id_string_) const -> pmt::rt::AstId::IdType;

 void for_each_id(std::invocable<std::string const&, pmt::rt::AstId::IdType> auto&& func_) const;
};

}  // namespace pmt::meta

#include "pmt/meta/id_table-inl.hpp"