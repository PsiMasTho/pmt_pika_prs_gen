#include "pmt/util/smrt/state_machine_tables_base.hpp"

namespace pmt::util::smrt {
using namespace pmt::base;

auto StateMachineTablesBase::get_accept_string(size_t index_) const -> std::string {
  return std::to_string(index_);
}

auto StateMachineTablesBase::get_accept_id(size_t) const -> GenericId::IdType {
  return GenericId::IdUninitialized;
}

auto StateMachineTablesBase::id_to_string(GenericId::IdType id_) const -> std::string {
  return std::to_string(id_);
}

}  // namespace pmt::util::smrt