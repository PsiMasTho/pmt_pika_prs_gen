#pragma once

#include "pmt/parser/rt/parser_tables_base.hpp"
#include "pmt/util/sm/ct/state_machine.hpp"

#include <unordered_map>

namespace pmt::parser::builder {

class ParserTables : public pmt::parser::rt::ParserTablesBase {
 // -$ Types / Constants $-
 struct NonterminalData {
  std::string _name;
  std::string _display_name;
  GenericId::IdType _id;
  bool _merge : 1;
  bool _unpack : 1;
  bool _hide : 1;
 };

 // -$ Data $-
 std::unordered_map<pmt::util::sm::StateNrType, pmt::base::Bitset> _lookahead_accepts_bitsets;

 pmt::util::sm::ct::StateMachine _parser_state_machine;
 pmt::util::sm::ct::StateMachine _lookahead_state_machine;

 std::vector<NonterminalData> _nonterminal_data;

 std::unordered_map<pmt::util::sm::StateNrType, pmt::base::Bitset> _parser_terminal_transition_masks;
 std::unordered_map<pmt::util::sm::StateNrType, pmt::base::Bitset> _parser_hidden_terminal_transition_masks;
 std::unordered_map<pmt::util::sm::StateNrType, pmt::base::Bitset> _parser_conflict_transition_masks;
 std::unordered_map<pmt::util::sm::StateNrType, std::unordered_map<pmt::util::sm::StateNrType, pmt::base::Bitset>> _lookahead_terminal_transition_masks;

public:
 // -$ Functions $-
 // --$ Inherited: pmt::parser::rt::ParserTablesBase $--
 auto get_state_nr_next(pmt::util::sm::StateNrType state_nr_, pmt::util::sm::SymbolKindType kind_, pmt::util::sm::SymbolValueType symbol_) const -> pmt::util::sm::StateNrType override;
 auto get_state_terminal_transitions(pmt::util::sm::StateNrType state_nr_) const -> pmt::base::Bitset::ChunkSpanConst override;
 auto get_state_hidden_terminal_transitions(pmt::util::sm::StateNrType state_nr_) const -> pmt::base::Bitset::ChunkSpanConst override;
 auto get_state_conflict_transitions(pmt::util::sm::StateNrType state_nr_) const -> pmt::base::Bitset::ChunkSpanConst override;
 auto get_state_kind(pmt::util::sm::StateNrType state_nr_) const -> StateKind override;
 auto get_state_accept_index(pmt::util::sm::StateNrType state_nr_) const -> size_t override;
 auto get_eoi_accept_index() const -> size_t override;
 auto get_accept_count() const -> size_t override;
 auto get_conflict_count() const -> size_t override;

 auto get_accept_index_display_name(size_t index_) const -> std::string override;
 auto get_accept_index_unpack(size_t index_) const -> bool override;
 auto get_accept_index_hide(size_t index_) const -> bool override;
 auto get_accept_index_merge(size_t index_) const -> bool override;
 auto get_accept_index_id(size_t index_) const -> GenericId::IdType override;

 auto get_lookahead_state_nr_next(pmt::util::sm::StateNrType state_nr_, pmt::util::sm::SymbolValueType symbol_) const -> pmt::util::sm::StateNrType override;
 auto get_lookahead_state_terminal_transitions(pmt::util::sm::StateNrType state_nr_, pmt::util::sm::StateNrType state_nr_parser_) const -> pmt::base::Bitset::ChunkSpanConst override;
 auto get_lookahead_state_accepts(pmt::util::sm::StateNrType state_nr_) const -> pmt::base::Bitset::ChunkSpanConst override;

 // --$ Other $--
 void set_parser_state_machine(pmt::util::sm::ct::StateMachine state_machine_);
 void set_lookahead_state_machine(pmt::util::sm::ct::StateMachine state_machine_);

 auto get_parser_state_machine() const -> pmt::util::sm::ct::StateMachine const&;
 auto get_lookahead_state_machine() const -> pmt::util::sm::ct::StateMachine const&;

 auto get_accept_index_name(size_t index_) const -> std::string;

 void add_nonterminal_data(std::string name_, std::string display_name_, GenericId::IdType id_value_, bool merge_, bool unpack_, bool hide_);

 auto nonterminal_name_to_index(std::string_view name_) const -> std::optional<size_t>;

private:
 void fill_terminal_transition_masks();
 void fill_hidden_terminal_transition_masks();
 void fill_conflict_transition_masks();
 void fill_lookahead_terminal_transition_masks();
};

}  // namespace pmt::parser::builder