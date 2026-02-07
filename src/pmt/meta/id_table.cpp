#include "pmt/meta/id_table.hpp"

#include "pmt/rt/reserved_ids.hpp"

#include <stdexcept>

namespace pmt::meta {

void IdTable::insert_id(std::string const& id_string_) {
 if (!pmt::rt::ReservedIds::is_reserved_id(id_string_) && _id_string_to_value.find(id_string_) == _id_string_to_value.end()) {
  pmt::rt::IdType const new_id = static_cast<pmt::rt::IdType>(_id_value_to_string.size());
  _id_string_to_value[id_string_] = new_id;
  _id_value_to_string.push_back(id_string_);
 }
}

auto IdTable::id_to_string(pmt::rt::IdType id_) const -> std::string {
 if (pmt::rt::ReservedIds::is_reserved_id(id_)) {
  return pmt::rt::ReservedIds::id_to_string(id_);
 }

 if (id_ >= _id_value_to_string.size()) {
  throw std::runtime_error("Invalid id value: " + std::to_string(id_));
 }

 return _id_value_to_string[id_];
}

auto IdTable::string_to_id(std::string const& id_string_) const -> pmt::rt::IdType {
 if (pmt::rt::ReservedIds::is_reserved_id(id_string_)) {
  return pmt::rt::ReservedIds::string_to_id(id_string_);
 }

 if (auto const itr = _id_string_to_value.find(id_string_); itr != _id_string_to_value.end()) {
  return itr->second;
 } else {
  throw std::runtime_error("Invalid id string: " + id_string_);
 }
}

}  // namespace pmt::meta