#include "pmt/builder/terminal_dotfile_writer.hpp"

#include "pmt/builder/emitter_utils.hpp"
#include "pmt/util/timestamp.hpp"

#include <algorithm>
#include <cctype>
#include <iomanip>
#include <set>
#include <sstream>
#include <unordered_map>
#include <unordered_set>
#include <utility>

namespace pmt::builder {
using namespace pmt::container;
using namespace pmt::rt;

namespace {

constexpr std::string_view GRAPH_TITLE = "State Machine";

auto rgb_to_string(uint8_t r_, uint8_t g_, uint8_t b_) -> std::string {
 std::stringstream ss;
 ss << R"("#)" << std::hex << std::setfill('0') << std::setw(2) << static_cast<int>(r_) << std::setw(2) << static_cast<int>(g_) << std::setw(2) << static_cast<int>(b_) << R"(")";
 return ss.str();
}

auto is_symbol_printable(SymbolType symbol_) -> bool {
 // clang-format off
 static std::unordered_set<SymbolType> printable_symbols = {
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

 std::stringstream ss;
 ss << std::hex << static_cast<uint64_t>(symbol_);
 std::string ret = ss.str();
 std::transform(ret.begin(), ret.end(), ret.begin(), [](auto const c_) { return std::toupper(c_); });
 return "0x" + ret;
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

auto build_final_id_table(StateMachine const& state_machine_, TerminalDotfileWriter::FinalIdToStringFn const& final_id_to_string_fn_) -> std::string {
 std::unordered_map<FinalIdType, std::set<StateNrType>> final_ids;
 for (StateNrType const state_nr : state_machine_.get_state_nrs()) {
  State const& state = *state_machine_.get_state(state_nr);
  state.get_final_ids().for_each_key([&](FinalIdType final_id_) { final_ids[final_id_].insert(state_nr); });
 }

 std::string table;
 table += "FinalId:StateNrs\n";
 for (auto const& [final_id, state_nrs] : final_ids) {
  table += final_id_to_string_fn_(final_id) + ": ";
  std::string delim;
  for (StateNrType state_nr : state_nrs) {
   table += std::exchange(delim, ", ") + std::to_string(state_nr);
  }
  table += '\n';
 }
 return table;
}

}  // namespace

TerminalDotfileWriter::TerminalDotfileWriter(StateMachine const& state_machine_, std::ostream& os_graph_, std::ifstream& skel_file_, FinalIdToStringFn final_id_to_string_fn_)
 : _state_machine(state_machine_)
 , _os_graph(os_graph_)
 , _skel_file(skel_file_)
 , _final_id_to_string_fn(std::move(final_id_to_string_fn_)) {
 if (!_final_id_to_string_fn) {
  _final_id_to_string_fn = [](FinalIdType final_id_) {
   return std::to_string(final_id_);
  };
 }
}

void TerminalDotfileWriter::write_dot() {
 std::string graph = read_stream(_skel_file, "terminal dotfile skeleton");

 std::string accepting_nodes_replacement;
 {
  std::string delim;
  for (StateNrType const state_nr : _state_machine.get_state_nrs()) {
   if (_state_machine.get_state(state_nr)->get_final_ids().popcnt() != 0) {
    accepting_nodes_replacement += std::exchange(delim, " ") + std::to_string(state_nr);
   }
  }
 }

 std::string epsilon_edges_replacement;
 {
  std::string space;
  for (StateNrType const state_nr : _state_machine.get_state_nrs()) {
   _state_machine.get_state(state_nr)->get_epsilon_transitions().for_each_key([&](StateNrType state_nr_next_) { epsilon_edges_replacement += std::exchange(space, " ") + std::to_string(state_nr) + " -> " + std::to_string(state_nr_next_) + "\n"; });
  }
 }

 std::string symbol_edges_replacement;
 {
  std::string space;
  symbol_edges_replacement += std::exchange(space, " ") + "edge [color=" + rgb_to_string(0, 0, 0) + ", style=solid]\n";
  for (StateNrType const state_nr : _state_machine.get_state_nrs()) {
   State const& state = *_state_machine.get_state(state_nr);
   std::unordered_map<StateNrType, IntervalSet<SymbolType>> symbol_intervals_per_state_nr_next;
   state.get_symbols().for_each_key([&](SymbolType symbol_) { symbol_intervals_per_state_nr_next[state.get_symbol_transition(symbol_)].insert(Interval(symbol_)); });

   for (auto const& [state_nr_next, symbol_intervals] : symbol_intervals_per_state_nr_next) {
    std::string label = build_symbol_label(symbol_intervals);
    symbol_edges_replacement += " " + std::to_string(state_nr) + " -> " + std::to_string(state_nr_next) + " [label=\"" + label + "\"]\n";
   }
  }
 }

 _replacer.replace_skeleton_label(graph, "FINAL_ID_TABLE", build_final_id_table(_state_machine, _final_id_to_string_fn));
 _replacer.replace_skeleton_label(graph, "TIMESTAMP", pmt::util::get_timestamp());
 _replacer.replace_skeleton_label(graph, "LAYOUT_DIRECTION", "LR");
 _replacer.replace_skeleton_label(graph, "ACCEPTING_NODE_SHAPE", "doublecircle");
 _replacer.replace_skeleton_label(graph, "ACCEPTING_NODE_COLOR", rgb_to_string(0, 0, 255));
 _replacer.replace_skeleton_label(graph, "ACCEPTING_NODES", accepting_nodes_replacement);
 _replacer.replace_skeleton_label(graph, "NONACCEPTING_NODE_SHAPE", "circle");
 _replacer.replace_skeleton_label(graph, "NONACCEPTING_NODE_COLOR", rgb_to_string(0, 0, 0));
 _replacer.replace_skeleton_label(graph, "EPSILON_EDGE_COLOR", rgb_to_string(0, 255, 0));
 _replacer.replace_skeleton_label(graph, "EPSILON_EDGES", epsilon_edges_replacement);
 _replacer.replace_skeleton_label(graph, "SYMBOL_EDGES", symbol_edges_replacement);
 _replacer.replace_skeleton_label(graph, "GRAPH_TITLE", std::string(GRAPH_TITLE));

 _os_graph << graph;
}

}  // namespace pmt::builder
