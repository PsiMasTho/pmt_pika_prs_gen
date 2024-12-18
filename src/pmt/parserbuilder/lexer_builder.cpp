#include "pmt/parserbuilder/lexer_builder.hpp"

#include "pmt/base/dynamic_bitset.hpp"
#include "pmt/base/dynamic_bitset_converter.hpp"
#include "pmt/parserbuilder/fa_part.hpp"
#include "pmt/parserbuilder/grm_ast.hpp"
#include "pmt/parserbuilder/grm_number.hpp"
#include "pmt/parserbuilder/lexer_table_transition_converter.hpp"
#include "pmt/parserbuilder/terminal_definition_to_fa_part.hpp"
#include "pmt/util/parsect/fa.hpp"
#include "pmt/util/parsect/graph_writer.hpp"
#include "pmt/util/parsert/generic_ast.hpp"

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <unordered_set>
#include <utility>
#include <vector>

namespace pmt::parserbuilder {
using namespace pmt::util::parsect;
using namespace pmt::util::parsert;

namespace {}  // namespace

LexerBuilder::LexerBuilder(GenericAst const& ast_, std::set<std::string> const& accepting_terminals_) {
  // Find terminal definitions
  for (size_t i = 0; i < ast_.get_children_size(); ++i) {
    GenericAst const& child = *ast_.get_child_at(i);
    if (child.get_id() == GrmAst::NtTerminalProduction) {
      std::string const& terminal_name = child.get_child_at(0)->get_string();
      std::string const& terminal_id = child.get_child_at(1)->get_string();

      if (accepting_terminals_.contains(terminal_name)) {
        _accepting_terminals.emplace_back(terminal_name);
      }

      _id_names_to_terminal_names[terminal_id].insert(terminal_name);

      if (auto const itr = _terminal_definitions.find(terminal_name); itr == _terminal_definitions.end()) {
        _terminal_definitions.insert_or_assign(terminal_name, GenericAst::PositionConst{&child, 2});
      } else {
        throw std::runtime_error("Terminal '" + terminal_name + "' defined multiple times");
      }
    }
  }

  std::ranges::sort(_accepting_terminals);

  // Check if all accepting terminals are defined
  std::unordered_set<std::string const*> missing_terminals;
  for (std::string const& terminal_name : accepting_terminals_) {
    if (!_terminal_definitions.contains(terminal_name)) {
      missing_terminals.insert(&terminal_name);
    }
  }

  if (!missing_terminals.empty()) {
    std::string text = "Missing terminal definition(s) for: ";
    std::string delim;
    for (std::string const* terminal_name : missing_terminals) {
      text += std::exchange(delim, ", ") + *terminal_name;
    }
    throw std::runtime_error(text);
  }

  _terminal_ids.resize(_accepting_terminals.size(), GenericId::IdDefault);
  for (GenericId::IdType i = 0; auto const& [terminal_id, terminal_names] : _id_names_to_terminal_names) {
    for (std::string const& terminal_name : terminal_names) {
      std::optional<size_t> const terminal_nr = find_accepting_terminal_nr(terminal_name);
      if (!terminal_nr.has_value()) {
        continue;
      }
      _terminal_ids[*terminal_nr] = (terminal_id == "IdDefault") ? GenericId::IdDefault : i;
    }
    i += (terminal_id == "IdDefault") ? 0 : 1;
  }
}

auto LexerBuilder::build() -> GenericLexerTables {
  Fa fa = build_initial_fa();
  write_dot(fa);
  fa.determinize();
  write_dot(fa);
  fa.minimize();
  write_dot(fa);

  pmt::base::DynamicBitset accepts0 = fa._states.find(0)->second._accepts;
  if (accepts0.any()) {
    static size_t const MAX_REPORTED = 8;
    std::unordered_set<size_t> const accepts0_set = pmt::base::DynamicBitsetConverter::to_unordered_set(accepts0);
    std::string msg;
    std::string delim;
    for (size_t i = 1; size_t const terminal_nr : accepts0_set) {
      msg += std::exchange(delim, ", ");
      if (i <= MAX_REPORTED) {
        msg += _accepting_terminals[terminal_nr];
      } else {
        msg += "...";
        break;
      }
    }
    throw std::runtime_error("Initial state accepts terminal(s): " + msg);
  }

  return fa_to_lexer_tables(fa);
}

auto LexerBuilder::build_initial_fa() -> Fa {
  Fa ret;

  Fa::State& state_start = ret._states[ret.get_unused_state_nr()];

  for (std::string const& terminal_name : _accepting_terminals) {
    GenericAst::PositionConst terminal_def = _terminal_definitions.find(terminal_name)->second;

    Fa::StateNrType state_nr_terminal_start = ret.get_unused_state_nr();
    Fa::State& state_terminal_start = ret._states[state_nr_terminal_start];
    state_start._transitions._epsilon_transitions.insert(state_nr_terminal_start);

    FaPart ret_part = TerminalDefinitionToFaPart::convert(ret, terminal_name, terminal_def, _terminal_definitions);

    state_terminal_start._transitions._epsilon_transitions.insert(*ret_part.get_incoming_state_nr());

    Fa::StateNrType state_nr_end = ret.get_unused_state_nr();
    Fa::State& state_end = ret._states[state_nr_end];
    state_end._accepts.resize(_accepting_terminals.size(), false);
    state_end._accepts.set(*find_accepting_terminal_nr(terminal_name), true);
    ret_part.connect_outgoing_transitions_to(state_nr_end, ret);
  }

  return ret;
}

auto LexerBuilder::fa_to_lexer_tables(Fa const& fa_) -> GenericLexerTables {
  GenericLexerTables ret;

  ret._accepts_width = _accepting_terminals.size();

  ret._accept_ids = _terminal_ids;
  ret._terminal_names = _accepting_terminals;

  for (auto const& [terminal_id, terminal_names] : _id_names_to_terminal_names) {
    if (terminal_id == "IdDefault") {
      continue;
    }
    ret._id_names.push_back(terminal_id);
  }

  LexerTableTransitionConverter::convert(fa_, ret);

  // We need to traverse the states in order
  std::set<Fa::StateNrType> state_nrs_sorted;
  for (auto const& [state_nr, state] : fa_._states) {
    state_nrs_sorted.insert(state_nr);
  }

  for (Fa::StateNrType const state_nr : state_nrs_sorted) {
    Fa::State const& state = fa_._states.find(state_nr)->second;

    size_t const accepts_width = state._accepts.size();
    assert(accepts_width == 0 || accepts_width == _accepting_terminals.size());
    if (accepts_width == 0) {
      ret._accepts.emplace_back(_accepting_terminals.size(), false);
    } else {
      ret._accepts.push_back(state._accepts);
    }
  }

  return ret;
}

auto LexerBuilder::find_accepting_terminal_nr(std::string const& terminal_name_) -> std::optional<size_t> {
  auto const itr = std::lower_bound(_accepting_terminals.begin(), _accepting_terminals.end(), terminal_name_);

  if (itr == _accepting_terminals.end() || *itr != terminal_name_) {
    return std::nullopt;
  }

  return std::distance(_accepting_terminals.begin(), itr);
}

void LexerBuilder::write_dot(Fa const& fa_) {
  if (fa_._states.size() > DOT_FILE_MAX_STATES) {
    std::cerr << "Skipping dot file write, too many states\n";
    return;
  }

  std::ofstream file(DOT_FILE_PREFIX + std::to_string(_dot_file_count++) + ".dot");
  GraphWriter::write_dot(file, fa_, [this](size_t accepts_) { return accepts_to_label(accepts_); });
}

auto LexerBuilder::accepts_to_label(size_t accepts_) -> std::string {
  return _accepting_terminals[accepts_];
}

}  // namespace pmt::parserbuilder