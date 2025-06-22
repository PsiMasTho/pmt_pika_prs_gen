#pragma once

#include "pmt/parser/generic_id.hpp"
#include "pmt/parser/rt/lexer_tables_base.hpp"
#include "pmt/util/sm/ct/state_machine.hpp"
#include "pmt/util/sm/primitives.hpp"

#include <string>

namespace pmt::parser::builder {

class LexerTables : public pmt::parser::rt::LexerTablesBase {
  // -$ Types / Constants $-
  struct TerminalData {
    std::string _name;
    std::string _display_name;
    GenericId::IdType _id;
  };

  // -$ Data $-
  pmt::util::sm::ct::StateMachine _lexer_state_machine;
  pmt::util::sm::ct::StateMachine _linecount_state_machine;

  std::unordered_map<pmt::util::sm::StateNrType, pmt::base::Bitset> _accepts_bitsets;
  std::vector<TerminalData> _terminal_data;

 public:
  // --$ Inherited: pmt::util::sm::LexerTablesBase $--
  auto get_state_nr_next(pmt::util::sm::StateNrType state_nr_, pmt::util::sm::SymbolValueType symbol_) const -> pmt::util::sm::StateNrType override;
  auto get_state_accepts(pmt::util::sm::StateNrType state_nr_) const -> pmt::base::Bitset::ChunkSpanConst override;
  auto get_accept_count() const -> size_t override;
  auto get_start_accept_index() const -> size_t override;
  auto get_eoi_accept_index() const -> size_t override;
  auto get_accept_index_display_name(size_t index_) const -> std::string override;
  auto get_accept_index_id(size_t index_) const -> GenericId::IdType override;
  auto get_linecount_state_nr_next(pmt::util::sm::StateNrType state_nr_, pmt::util::sm::SymbolValueType symbol_) const -> pmt::util::sm::StateNrType override;
  auto is_linecount_state_nr_accepting(pmt::util::sm::StateNrType state_nr_) const -> bool override;

  // --$ Other $--
  void set_lexer_state_machine(pmt::util::sm::ct::StateMachine state_machine_);
  void set_linecount_state_machine(pmt::util::sm::ct::StateMachine state_machine_);

  auto get_lexer_state_machine() const -> pmt::util::sm::ct::StateMachine const&;
  auto get_linecount_state_machine() const -> pmt::util::sm::ct::StateMachine const&;

  auto get_accept_index_name(size_t index_) const -> std::string;

  void add_terminal_data(std::string name_, std::string display_name_, GenericId::IdType id_value_);

  auto terminal_name_to_index(std::string_view name_) const -> std::optional<size_t>;
};

}  // namespace pmt::parser::builder