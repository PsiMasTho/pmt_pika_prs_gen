#include "pmt/pika/meta/id_table.hpp"

#include "pmt/pika/rt/reserved_ids.hpp"

#include <stdexcept>

namespace pmt::pika::meta {

void IdTable::insert_id(std::string const& id_string_) {
 if (!pmt::pika::rt::ReservedIds::is_reserved_id(id_string_) && _id_string_to_value.find(id_string_) == _id_string_to_value.end()) {
  pmt::pika::rt::IdType const new_id = static_cast<pmt::pika::rt::IdType>(_id_value_to_string.size());
  _id_string_to_value[id_string_] = new_id;
  _id_value_to_string.push_back(id_string_);
 }
}

auto IdTable::id_to_string(pmt::pika::rt::IdType id_) const -> std::string {
 if (pmt::pika::rt::ReservedIds::is_reserved_id(id_)) {
  return pmt::pika::rt::ReservedIds::id_to_string(id_);
 }

 if (id_ >= _id_value_to_string.size()) {
  throw std::runtime_error("Invalid id value: " + std::to_string(id_));
 }

 return _id_value_to_string[id_];
}

auto IdTable::string_to_id(std::string const& id_string_) const -> pmt::pika::rt::IdType {
 if (pmt::pika::rt::ReservedIds::is_reserved_id(id_string_)) {
  return pmt::pika::rt::ReservedIds::string_to_id(id_string_);
 }

 if (auto const itr = _id_string_to_value.find(id_string_); itr != _id_string_to_value.end()) {
  return itr->second;
 } else {
  throw std::runtime_error("Invalid id string: " + id_string_);
 }
}

}  // namespace pmt::pika::meta