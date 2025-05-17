#pragma once

#include "pmt/util/smct/state_machine.hpp"
#include "pmt/util/smrt/parser_tables_base.hpp"

namespace pmt::parserbuilder {

class ParserTables : public pmt::util::smrt::ParserTablesBase {
 public:
  // -$ Types / Constants $-
  struct NonterminalData {
    std::string _label;
    pmt::util::smrt::GenericId::IdType _id;
  };

  // -$ Data $-
  pmt::util::smct::StateMachine _parser_state_machine;
  pmt::util::smct::StateMachine _lookahead_state_machine;

  std::vector<std::string> _id_names;
  std::vector<NonterminalData> _nonterminal_data;

  // --$ Inherited: pmt::util::smrt::parserTablesBase $--
  auto get_state_nr_next(pmt::util::smrt::StateNrType state_nr_, pmt::util::smrt::SymbolType symbol_) const -> pmt::util::smrt::StateNrType override;
  auto get_state_nonterminal(pmt::util::smrt::StateNrType state_nr_) const -> size_t override;
  auto get_nonterminal_count() const -> size_t override;
  auto get_nonterminal_label(size_t index_) const -> std::string override;
  auto get_nonterminal_id(size_t index_) const -> pmt::util::smrt::GenericId::IdType override;
  auto id_to_string(pmt::util::smrt::GenericId::IdType id_) const -> std::string override;
};

}