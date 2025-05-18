#pragma once

#include "pmt/util/smct/state_machine.hpp"
#include "pmt/util/smrt/lexer_tables_base.hpp"
#include "pmt/util/smrt/generic_id.hpp"

#include <string>

namespace pmt::parserbuilder {

class LexerTables : public pmt::util::smrt::LexerTablesBase {
 public:
  // -$ Types / Constants $-
  struct TerminalData {
    std::string _label;
    pmt::util::smrt::GenericId::IdType _id;
    bool _hide;
  };

  // -$ Data $-
  pmt::util::smct::StateMachine _lexer_state_machine;
  pmt::util::smct::StateMachine _linecount_state_machine;

  std::vector<std::string> _id_names;
  std::vector<TerminalData> _terminal_data;
  size_t _start_accept_index;
  size_t _eoi_accept_index;
  size_t _id_count;

  // --$ Inherited: pmt::util::smrt::LexerTablesBase $--
  auto get_state_nr_next(pmt::util::smrt::StateNrType state_nr_, pmt::util::smrt::SymbolType symbol_) const -> pmt::util::smrt::StateNrType override;
  auto get_state_accepts(pmt::util::smrt::StateNrType state_nr_) const -> pmt::base::Bitset::ChunkSpanConst override;
  auto get_accept_count() const -> size_t override;
  auto get_start_accept_index() const -> size_t override;
  auto get_eoi_accept_index() const -> size_t override;
  auto get_accept_index_label(size_t index_) const -> std::string override;
  auto get_accept_index_hide(size_t index_) const -> bool override;
  auto get_accept_index_id(size_t index_) const -> pmt::util::smrt::GenericId::IdType override;
  auto id_to_string(pmt::util::smrt::GenericId::IdType id_) const -> std::string override;
  auto get_id_count() const -> size_t override;
  auto get_linecount_state_nr_next(pmt::util::smrt::StateNrType state_nr_, pmt::util::smrt::SymbolType symbol_) const -> pmt::util::smrt::StateNrType override;
  auto is_linecount_state_nr_accepting(pmt::util::smrt::StateNrType state_nr_) const -> bool override;

  // --$ Other $--
  auto terminal_label_to_index(std::string_view label_) const -> std::optional<size_t>;

};

}  // namespace pmt::parserbuilder