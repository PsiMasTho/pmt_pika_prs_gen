#include "pmt/parser/grammar/id_table.hpp"

#include <stdexcept>

namespace pmt::parser::grammar {

void IdTable::insert_id(std::string const& id_string_) {
 if (!GenericId::is_generic_id(id_string_) && _id_string_to_value.find(id_string_) == _id_string_to_value.end()) {
  GenericId::IdType const new_id = static_cast<GenericId::IdType>(_id_value_to_string.size());
  _id_string_to_value[id_string_] = new_id;
  _id_value_to_string.push_back(id_string_);
 }
}

auto IdTable::id_to_string(GenericId::IdType id_) const -> std::string {
 if (GenericId::is_generic_id(id_)) {
  return GenericId::id_to_string(id_);
 }

 if (id_ >= _id_value_to_string.size()) {
  throw std::runtime_error("Invalid id value: " + std::to_string(id_));
 }

 return _id_value_to_string[id_];
}

auto IdTable::string_to_id(std::string const& id_string_) const -> GenericId::IdType {
 if (GenericId::is_generic_id(id_string_)) {
  return GenericId::string_to_id(id_string_);
 }

 if (auto const itr = _id_string_to_value.find(id_string_); itr != _id_string_to_value.end()) {
  return itr->second;
 } else {
  throw std::runtime_error("Invalid id string: " + id_string_);
 }
}

}  // namespace pmt::parser::grammar