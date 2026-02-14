#include "pmt/builder/terminal_fsm_to_dot_str.hpp"

#include "pmt/container/interval_set.hpp"
#include "pmt/sm/state_machine.hpp"
#include "pmt/util/get_timestamp_str.hpp"
#include "pmt/util/singleton.hpp"
#include "pmt/util/skeleton_replacer.hpp"
#include "pmt/util/uint_to_str.hpp"

#include <set>
#include <unordered_map>
#include <unordered_set>
#include <utility>

namespace pmt::builder {
using namespace pmt::util;
using namespace pmt::rt;
using namespace pmt::container;

namespace {

auto rgb_to_str(uint8_t r_, uint8_t g_, uint8_t b_) -> std::string {
 // clang-format off
 return "\"#" +
    uint_to_string(r_, 2, hex_alphabet_uppercase)
  + uint_to_string(g_, 2, hex_alphabet_uppercase)
  + uint_to_string(b_, 2, hex_alphabet_uppercase) + "\"";
 // clang-format on
}

auto is_symbol_printable(SymbolType symbol_) -> bool {
 // clang-format off
 static std::unordered_set<SymbolType> const printable_symbols = {
  ' ', '!', '#', '$', '%', '&', '\'', '(', ')', '*', '+', ',', '-', '.', '/',
  '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
  ':', ';', '<', '=', '>', '?', '@',
  'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M',
  'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
  '[', '\\', ']', '^', '_', '`',
  'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i',  'j',  'k',  'l',  'm',
  'n', 'o', 'p', 'q', 'r', 's',  't',  'u',  'v',  'w',  'x',  'y',  'z',
  '{', '|', '}', '~'
 };
 // clang-format on
 return printable_symbols.contains(symbol_);
}

auto to_displayable(SymbolType symbol_) -> std::string {
 if (is_symbol_printable(symbol_)) {
  return R"(')" + std::string(1, symbol_) + R"(')";
 }
 return "0x" + uint_to_string(symbol_, 0, hex_alphabet_uppercase);
}

auto interval_to_label(Interval<SymbolType> interval_) -> std::string {
 if (interval_.get_lower() == interval_.get_upper()) {
  return to_displayable(interval_.get_lower());
 }
 return "[" + to_displayable(interval_.get_lower()) + ".." + to_displayable(interval_.get_upper()) + "]";
}

auto build_symbol_label(IntervalSet<SymbolType> const& symbol_intervals_) -> std::string {
 std::string label;
 std::string delim;
 symbol_intervals_.for_each_interval([&](Interval<SymbolType> interval_) { label += std::exchange(delim, ", ") + interval_to_label(interval_); });
 return label;
}

auto make_final_id_table_string(pmt::sm::StateMachine const& state_machine_, FinalIdToStringFn const& final_id_to_str_) -> std::string {
 std::unordered_map<IdType, std::set<StateNrType>> final_ids;
 for (StateNrType const state_nr : state_machine_.get_state_nrs()) {
  pmt::sm::State const& state = *state_machine_.get_state(state_nr);
  state.get_final_ids().for_each_key([&](IdType final_id_) { final_ids[final_id_].insert(state_nr); });
 }

 std::string table;
 table += "FinalId:StateNrs\n";
 for (auto const& [final_id, state_nrs] : final_ids) {
  table += final_id_to_str_(final_id) + ": ";
  std::string delim;
  for (StateNrType state_nr : state_nrs) {
   table += std::exchange(delim, ", ") + std::to_string(state_nr);
  }
  table += '\n';
 }
 return table;
}

}  // namespace

auto terminal_fsm_to_dot_str(pmt::sm::StateMachine const& state_machine_, FinalIdToStringFn const& final_id_to_str_, std::string terminal_dotfile_skel_) -> std::string {
 auto skeleton_replacer = Singleton<SkeletonReplacer>::instance();

 std::string accepting_nodes_replacement;
 {
  std::string delim;
  for (StateNrType const state_nr : state_machine_.get_state_nrs()) {
   if (state_machine_.get_state(state_nr)->get_final_ids().popcnt() != 0) {
    accepting_nodes_replacement += std::exchange(delim, " ") + std::to_string(state_nr);
   }
  }
 }

 std::string epsilon_edges_replacement;
 {
  std::string space;
  for (StateNrType const state_nr : state_machine_.get_state_nrs()) {
   state_machine_.get_state(state_nr)->get_epsilon_transitions().for_each_key([&](StateNrType state_nr_next_) { epsilon_edges_replacement += std::exchange(space, " ") + std::to_string(state_nr) + " -> " + std::to_string(state_nr_next_) + "\n"; });
  }
 }

 std::string symbol_edges_replacement;
 {
  std::string space;
  symbol_edges_replacement += std::exchange(space, " ") + "edge [color=" + rgb_to_str(0, 0, 0) + ", style=solid]\n";
  for (StateNrType const state_nr : state_machine_.get_state_nrs()) {
   pmt::sm::State const& state = *state_machine_.get_state(state_nr);
   std::unordered_map<StateNrType, IntervalSet<SymbolType>> symbol_intervals_per_state_nr_next;
   state.get_symbols().for_each_key([&](SymbolType symbol_) { symbol_intervals_per_state_nr_next[state.get_symbol_transition(symbol_)].insert(Interval(symbol_)); });

   for (auto const& [state_nr_next, symbol_intervals] : symbol_intervals_per_state_nr_next) {
    std::string label = build_symbol_label(symbol_intervals);
    symbol_edges_replacement += " " + std::to_string(state_nr) + " -> " + std::to_string(state_nr_next) + " [label=\"" + label + "\"]\n";
   }
  }
 }

 skeleton_replacer->replace_label(terminal_dotfile_skel_, "FINAL_ID_TABLE", make_final_id_table_string(state_machine_, final_id_to_str_));
 skeleton_replacer->replace_label(terminal_dotfile_skel_, "TIMESTAMP", get_timestamp_str());
 skeleton_replacer->replace_label(terminal_dotfile_skel_, "LAYOUT_DIRECTION", "LR");
 skeleton_replacer->replace_label(terminal_dotfile_skel_, "ACCEPTING_NODE_SHAPE", "doublecircle");
 skeleton_replacer->replace_label(terminal_dotfile_skel_, "ACCEPTING_NODE_COLOR", rgb_to_str(0, 0, 255));
 skeleton_replacer->replace_label(terminal_dotfile_skel_, "ACCEPTING_NODES", accepting_nodes_replacement);
 skeleton_replacer->replace_label(terminal_dotfile_skel_, "NONACCEPTING_NODE_SHAPE", "circle");
 skeleton_replacer->replace_label(terminal_dotfile_skel_, "NONACCEPTING_NODE_COLOR", rgb_to_str(0, 0, 0));
 skeleton_replacer->replace_label(terminal_dotfile_skel_, "EPSILON_EDGE_COLOR", rgb_to_str(0, 255, 0));
 skeleton_replacer->replace_label(terminal_dotfile_skel_, "EPSILON_EDGES", epsilon_edges_replacement);
 skeleton_replacer->replace_label(terminal_dotfile_skel_, "SYMBOL_EDGES", symbol_edges_replacement);
 skeleton_replacer->replace_label(terminal_dotfile_skel_, "GRAPH_TITLE", "Terminal State Machine");

 return terminal_dotfile_skel_;
}

}  // namespace pmt::builder
