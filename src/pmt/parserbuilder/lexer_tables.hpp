#pragma once

#include "pmt/util/smct/state_machine.hpp"
#include "pmt/util/smrt/generic_id.hpp"

#include <string>

namespace pmt::parserbuilder {

class LexerTables : public util::smct::StateMachine {
 public:
  // -$ Types / Constants $-
  struct TerminalData {
    std::string _name;
    pmt::util::smrt::GenericId::IdType _id;
  };

  // -$ Data $-
  std::unordered_map<pmt::util::smrt::GenericId::IdType, std::string> _id_to_string;
  std::vector<TerminalData> _terminal_data;

  // --$ Inherited: pmt::util::smrt::StateMachine $--
  auto get_accept_string(size_t index_) const -> std::string override;
  auto get_accept_id(size_t index_) const -> pmt::util::smrt::GenericId::IdType override;
  auto id_to_string(pmt::util::smrt::GenericId::IdType id_) const -> std::string override;
};

}  // namespace pmt::parserbuilder