#pragma once

#include "pmt/parser/generic_id.hpp"

#include <concepts>
#include <string>
#include <unordered_map>
#include <vector>

namespace pmt::parser::grammar {

class IdTable {
 std::unordered_map<std::string, GenericId::IdType> _id_string_to_value;
 std::vector<std::string> _id_value_to_string;

public:
 void insert_id(std::string const& id_string_);

 auto id_to_string(GenericId::IdType id_) const -> std::string;
 auto string_to_id(std::string const& id_string_) const -> GenericId::IdType;

 void for_each_id(std::invocable<std::string const&, GenericId::IdType> auto&& func_) const;
};

}  // namespace pmt::parser::grammar

#include "pmt/parser/grammar/id_table-inl.hpp"