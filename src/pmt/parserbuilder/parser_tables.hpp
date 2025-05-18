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
    bool _merge : 1;
    bool _unpack : 1;
    bool _hide : 1;
  };

  // -$ Data $-
  pmt::util::smct::StateMachine _parser_state_machine;
  pmt::util::smct::StateMachine _lookahead_state_machine;

  std::vector<std::string> _id_names;
  std::vector<NonterminalData> _nonterminal_data;

  std::unordered_map<pmt::util::smrt::StateNrType, pmt::base::Bitset> _parser_terminal_transition_masks;
  std::unordered_map<pmt::util::smrt::StateNrType, pmt::base::Bitset> _parser_conflict_transition_masks;
  std::unordered_map<pmt::util::smrt::StateNrType, pmt::base::Bitset> _lookahead_terminal_transition_masks;

  pmt::util::smrt::GenericId::IdType _min_id;
  size_t _id_count;
  size_t _conflict_count;
  size_t _eoi_accept_index;

  // --$ Inherited: pmt::util::smrt::ParserTablesBase $--
  auto get_state_nr_next(pmt::util::smrt::StateNrType state_nr_, pmt::util::smrt::SymbolType symbol_) const -> pmt::util::smrt::StateNrType override;
  auto get_state_terminal_transitions(pmt::util::smrt::StateNrType state_nr_) const -> pmt::base::Bitset::ChunkSpanConst override;
  auto get_state_conflict_transitions(pmt::util::smrt::StateNrType state_nr_) const -> pmt::base::Bitset::ChunkSpanConst override;
  auto get_state_kind(pmt::util::smrt::StateNrType state_nr_) const -> StateKind override;
  auto get_state_accept_index(pmt::util::smrt::StateNrType state_nr_) const -> size_t override;
  auto get_eoi_accept_index() const -> size_t override;
  auto get_accept_count() const -> size_t override;
  auto get_conflict_count() const -> size_t override;

  auto get_accept_index_label(size_t index_) const -> std::string override;
  auto get_accept_index_unpack(size_t index_) const -> bool override;
  auto get_accept_index_hide(size_t index_) const -> bool override;
  auto get_accept_index_merge(size_t index_) const -> bool override;
  auto get_accept_index_id(size_t index_) const -> pmt::util::smrt::GenericId::IdType override;
  auto id_to_string(pmt::util::smrt::GenericId::IdType id_) const -> std::string override;
  auto get_min_id() const -> pmt::util::smrt::GenericId::IdType override;
  auto get_id_count() const -> size_t override;

  auto get_lookahead_state_nr_next(pmt::util::smrt::StateNrType state_nr_, pmt::util::smrt::SymbolType symbol_) const -> pmt::util::smrt::StateNrType override;
  auto get_lookahead_state_terminal_transitions(pmt::util::smrt::StateNrType state_nr_) const -> pmt::base::Bitset::ChunkSpanConst override;
  auto get_lookahead_state_accepts(pmt::util::smrt::StateNrType state_nr_) const -> pmt::base::Bitset::ChunkSpanConst override;
};

}