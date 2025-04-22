#include "pmt/util/smct/graph_writer.hpp"

#include "pmt/base/bitset.hpp"
#include "pmt/base/bitset_converter.hpp"
#include "pmt/util/smct/state_machine.hpp"

#include <ios>
#include <iostream>
#include <sstream>
#include <unordered_map>
#include <utility>

namespace pmt::util::smct {
using namespace pmt::base;
using namespace pmt::util::smrt;

void GraphWriter::write_dot(std::ostream& os_, StateMachine const& state_machine_, AcceptsToLabel accepts_to_label_) {
  if (!accepts_to_label_) {
    accepts_to_label_ = accepts_to_label_default;
  }

  // <state_nr> -> <state_nr_next, label>
  std::unordered_map<StateNrType, std::unordered_map<StateNrType, std::string>> symbol_arrow_labels;

  IntervalSet<StateNrType> const state_nrs = state_machine_.get_state_nrs();

  state_nrs.for_each_key([&](StateNrType state_nr_) {
    State const& state = *state_machine_.get_state(state_nr_);

    // Symbol transitions
    IntervalSet<SymbolType> const symbols = state.get_symbols();

    if (!symbols.empty()) {
      std::unordered_map<StateNrType, IntervalSet<SymbolType>> relations;

      symbols.for_each_key([&](SymbolType symbol_) {
        StateNrType const state_nr_next = state.get_symbol_transition(Symbol(symbol_));
        relations[state_nr_next].insert(Interval<SymbolType>(symbol_));
      });

      for (auto const& [state_nr_next, symbols] : relations) {
        symbol_arrow_labels[state_nr_][state_nr_next] = create_label(symbols);
      }
    }
  });

  // - Write the dot file -
  std::string delim;
  os_ << "digraph finite_state_machine {\n"
         " rankdir=LR;\n"
         " subgraph cluster_states {\n"
         " peripheries=0;\n"
         "  node [shape=doublecircle, color=blue]; ";

  state_nrs.for_each_key([&](StateNrType state_nr_) {
    if (state_machine_.get_state(state_nr_)->get_accepts().popcnt() != 0) {
      os_ << std::exchange(delim, " ") << state_nr_;
    }
  });

  if (!delim.empty()) {
    os_ << ";\n";
  }
  os_ << "  node [shape=circle, color=black];\n";

  // Epsilon transitions
  os_ << "  edge [color=green];\n";
  state_nrs.for_each_key([&](StateNrType state_nr_) { state_machine_.get_state(state_nr_)->get_epsilon_transitions().for_each_key([&](StateNrType state_nr_next_) { os_ << "  " << state_nr_ << " -> " << state_nr_next_ << "\n"; }); });

  // Symbol transitions
  os_ << "  edge [color=black];\n";
  for (auto const& [state_nr, state_nr_next_and_label] : symbol_arrow_labels) {
    for (auto const& [state_nr_next, label] : state_nr_next_and_label) {
      os_ << "  " << state_nr << " -> " << state_nr_next << " [label=\"" << label << "\"]\n";
    }
  }

  os_ << " }\n";

  // Accepts
  std::unordered_map<size_t, std::set<StateNrType>> accepts;
  state_nrs.for_each_key([&](StateNrType state_nr_) {
    State const& state = *state_machine_.get_state(state_nr_);
    std::set<size_t> const accepts_set = pmt::base::BitsetConverter::to_set(state.get_accepts());
    for (size_t accept : accepts_set) {
      accepts[accept].insert(state_nr_);
    }
  });

  os_ << "\n"
         " subgraph cluster_accepts {\n"
         "  label=\"Accepts\";\n"
         "  color=gray;\n"
         "  style=filled;\n\n"
         "  node [shape=record;style=filled;fillcolor=white];\n\n";

  os_ << "  node_accepts [label=\"";
  delim.clear();
  for (auto const& [accept, state_nrs_accepted] : accepts) {
    std::string const lhs = accepts_to_label_(accept);
    os_ << std::exchange(delim, "|") << lhs << ": ";

    std::string delim2;
    for (StateNrType state_nr : state_nrs_accepted) {
      os_ << std::exchange(delim2, ", ") << state_nr;
    }
    os_ << R"(\l)";
  }
  os_ << "\"];\n\n"
         " }\n"
         "}\n";
}

auto GraphWriter::accepts_to_label_default(size_t accepts_) -> std::string {
  return std::to_string(accepts_);
}

auto GraphWriter::create_label(pmt::base::IntervalSet<SymbolType> const& symbols_) -> std::string {
  std::string ret;
  std::string delim;

  symbols_.for_each_interval([&](Interval<SymbolType> interval_) {
    ret += std::exchange(delim, ", ");
    if (interval_.get_lower() == interval_.get_upper()) {
      ret += to_displayable(interval_.get_lower());
    } else {
      ret += "[" + to_displayable(interval_.get_lower()) + ".." + to_displayable(interval_.get_upper()) + "]";
    }
  });

  return ret;
}

auto GraphWriter::is_displayable(SymbolType symbol_) -> bool {
  static std::unordered_set<SymbolType> const DISPLAYABLES = {
   // clang-format off
      '!', '#', '$', '%', '&', '(', ')', '*', ',', '-', '.', '@', '^', '_',
      '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
      'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
      'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',
   // clang-format on
  };

  return DISPLAYABLES.contains(symbol_);
}

auto GraphWriter::to_displayable(SymbolType symbol_) -> std::string {
  if (is_displayable(symbol_)) {
    return R"(')" + std::string(1, symbol_) + R"(')";
  } else if (symbol_ == SymbolEoi) {
    return "<EOI>";
  }

  std::stringstream ss;
  ss << std::hex << symbol_;
  std::string ret = ss.str();
  std::transform(ret.begin(), ret.end(), ret.begin(), [](auto const c_) { return std::toupper(c_); });
  return "0x" + ret;
}

}  // namespace pmt::util::smct