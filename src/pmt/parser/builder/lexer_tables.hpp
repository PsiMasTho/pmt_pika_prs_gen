#pragma once

#include "pmt/util/sm/ct/state_machine.hpp"
#include "pmt/util/sm/primitives.hpp"
#include "pmt/parser/rt/lexer_tables_base.hpp"
#include "pmt/parser/generic_id.hpp"

#include <string>
#include <map>

namespace pmt::parser::builder {

class LexerTables : public pmt::parser::rt::LexerTablesBase {
  // -$ Types / Constants $-
  struct TerminalData {
    std::string _label;
    GenericId::IdType _id;
  };

  // -$ Data $-
  pmt::util::sm::ct::StateMachine _lexer_state_machine;
  pmt::util::sm::ct::StateMachine _linecount_state_machine;

  std::unordered_map<pmt::util::sm::StateNrType, pmt::base::Bitset> _accepts_bitsets;
  std::map<GenericId::IdType, std::string> _id_to_name;
  std::unordered_map<std::string, GenericId::IdType> _name_to_id;
  std::vector<TerminalData> _terminal_data;

  public:
  // --$ Inherited: pmt::util::sm::LexerTablesBase $--
  auto get_state_nr_next(pmt::util::sm::StateNrType state_nr_, pmt::util::sm::SymbolValueType symbol_) const -> pmt::util::sm::StateNrType override;
  auto get_state_accepts(pmt::util::sm::StateNrType state_nr_) const -> pmt::base::Bitset::ChunkSpanConst override;
  auto get_accept_count() const -> size_t override;
  auto get_start_accept_index() const -> size_t override;
  auto get_eoi_accept_index() const -> size_t override;
  auto get_accept_index_label(size_t index_) const -> std::string override;
  auto get_accept_index_id(size_t index_) const -> GenericId::IdType override;
  auto id_to_string(GenericId::IdType id_) const -> std::string override;
  auto get_min_id() const -> GenericId::IdType override;
  auto get_id_count() const -> size_t override;
  auto get_linecount_state_nr_next(pmt::util::sm::StateNrType state_nr_, pmt::util::sm::SymbolValueType symbol_) const -> pmt::util::sm::StateNrType override;
  auto is_linecount_state_nr_accepting(pmt::util::sm::StateNrType state_nr_) const -> bool override;

  // --$ Other $--
  void set_lexer_state_machine(pmt::util::sm::ct::StateMachine state_machine_);
  void set_linecount_state_machine(pmt::util::sm::ct::StateMachine state_machine_);

  auto get_lexer_state_machine() const -> pmt::util::sm::ct::StateMachine const&;
  auto get_linecount_state_machine() const -> pmt::util::sm::ct::StateMachine const&;

  void add_terminal_data(std::string label_, std::string const& id_name_);

  auto terminal_label_to_index(std::string_view label_) const -> std::optional<size_t>;

};

}  // namespace pmt::parserbuilder