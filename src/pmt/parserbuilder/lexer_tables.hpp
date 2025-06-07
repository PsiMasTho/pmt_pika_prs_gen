#pragma once

#include "pmt/util/smct/state_machine.hpp"
#include "pmt/util/smrt/lexer_tables_base.hpp"
#include "pmt/util/smrt/generic_id.hpp"

#include <string>
#include <map>

namespace pmt::parserbuilder {

class LexerTables : public pmt::util::smrt::LexerTablesBase {
  // -$ Types / Constants $-
  struct TerminalData {
    std::string _label;
    pmt::util::smrt::GenericId::IdType _id;
  };

  // -$ Data $-
  pmt::util::smct::StateMachine _lexer_state_machine;
  pmt::util::smct::StateMachine _linecount_state_machine;

  std::unordered_map<pmt::util::smrt::StateNrType, pmt::base::Bitset> _accepts_bitsets;
  std::map<pmt::util::smrt::GenericId::IdType, std::string> _id_to_name;
  std::unordered_map<std::string, pmt::util::smrt::GenericId::IdType> _name_to_id;
  std::vector<TerminalData> _terminal_data;

  public:
  // --$ Inherited: pmt::util::smrt::LexerTablesBase $--
  auto get_state_nr_next(pmt::util::smrt::StateNrType state_nr_, pmt::util::smrt::SymbolValueType symbol_) const -> pmt::util::smrt::StateNrType override;
  auto get_state_accepts(pmt::util::smrt::StateNrType state_nr_) const -> pmt::base::Bitset::ChunkSpanConst override;
  auto get_accept_count() const -> size_t override;
  auto get_start_accept_index() const -> size_t override;
  auto get_eoi_accept_index() const -> size_t override;
  auto get_accept_index_label(size_t index_) const -> std::string override;
  auto get_accept_index_id(size_t index_) const -> pmt::util::smrt::GenericId::IdType override;
  auto id_to_string(pmt::util::smrt::GenericId::IdType id_) const -> std::string override;
  auto get_min_id() const -> pmt::util::smrt::GenericId::IdType override;
  auto get_id_count() const -> size_t override;
  auto get_linecount_state_nr_next(pmt::util::smrt::StateNrType state_nr_, pmt::util::smrt::SymbolValueType symbol_) const -> pmt::util::smrt::StateNrType override;
  auto is_linecount_state_nr_accepting(pmt::util::smrt::StateNrType state_nr_) const -> bool override;

  // --$ Other $--
  void set_lexer_state_machine(pmt::util::smct::StateMachine state_machine_);
  void set_linecount_state_machine(pmt::util::smct::StateMachine state_machine_);

  auto get_lexer_state_machine() const -> pmt::util::smct::StateMachine const&;
  auto get_linecount_state_machine() const -> pmt::util::smct::StateMachine const&;

  void add_terminal_data(std::string label_, std::string const& id_name_);

  auto terminal_label_to_index(std::string_view label_) const -> std::optional<size_t>;

};

}  // namespace pmt::parserbuilder