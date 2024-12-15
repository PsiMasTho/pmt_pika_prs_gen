#include "pmt/parserbuilder/lexer_tables.hpp"

namespace pmt::parserbuilder {

auto LexerTables::fill_sv() -> void {
  _terminal_names_sv.reserve(_terminal_names.size());
  for (std::string const& terminal_name : _terminal_names) {
    _terminal_names_sv.push_back(terminal_name);
  }

  _id_names_sv.reserve(_id_names.size());
  for (std::string const& id_name : _id_names) {
    _id_names_sv.push_back(id_name);
  }
}

auto LexerTables::as_generic_lexer_tables() const -> pmt::util::parsert::GenericLexerTables {
  pmt::util::parsert::GenericLexerTables ret;
  ret._state_nr_sink = _state_nr_sink;
  ret._state_nr_min_diff = _state_nr_min_diff;

  ret._padding_l = _padding_l;

  ret._state_count = _state_transition_entries.size();
  ret._compressed_transition_entry_count = _compressed_transition_entries.size();
  ret._accepts_2d_width = _accepts_2d_width,

  ret._state_transition_entries = _state_transition_entries.data();
  ret._compressed_transition_entries = _compressed_transition_entries.data();

  ret._accepts_2d = _accepts_2d.data();
  ret._accept_ids = _accept_ids.data();

  ret._terminal_names = _terminal_names_sv.data();
  ret._id_names = _id_names_sv.data();

  return ret;
}

}  // namespace pmt::parserbuilder