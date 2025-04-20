#include "pmt/util/smct/graph_writer.hpp"

#include "pmt/base/bitset.hpp"
#include "pmt/base/bitset_converter.hpp"

#include <ios>
#include <iostream>
#include <sstream>
#include <unordered_map>
#include <utility>

namespace pmt::util::smct {
using namespace pmt::base;

void GraphWriter::write_dot(std::ostream& os_, StateMachine const& state_machine_, AcceptsToLabel accepts_to_label_) {
  if (!accepts_to_label_) {
    accepts_to_label_ = accepts_to_label_default;
  }

  // <state_nr> -> <state_nr_next, label>
  std::unordered_map<State::StateNrType, std::unordered_map<State::StateNrType, std::string>> symbol_arrow_labels;

  IntervalSet<State::StateNrType> const state_nrs = state_machine_.get_state_nrs();
  for (size_t i = 0; i < state_nrs.size(); ++i) {
    Interval<State::StateNrType> const& interval = state_nrs.get_by_index(i);
    for (State::StateNrType state_nr = interval.get_lower(); state_nr <= interval.get_upper(); ++state_nr) {
      State const& state = *state_machine_.get_state(state_nr);

      // Symbol transitions
      IntervalSet<Symbol::UnderlyingType> const symbols = state.get_symbols();

      if (symbols.empty()) {
        continue;
      }

      std::unordered_map<State::StateNrType, IntervalSet<Symbol::UnderlyingType>> relations;

      for (size_t i = 0; i < symbols.size(); ++i) {
        Interval<Symbol::UnderlyingType> const& symbol_interval = symbols.get_by_index(i);
        for (Symbol::UnderlyingType c = symbol_interval.get_lower(); c <= symbol_interval.get_upper(); ++c) {
          State::StateNrType const state_nr_next = state.get_symbol_transition(Symbol(c));
          relations[state_nr_next].insert(Interval<Symbol::UnderlyingType>(c));
        }
      }

      for (auto const& [state_nr_next, symbols] : relations) {
        symbol_arrow_labels[state_nr][state_nr_next] = create_label(symbols);
      }
    }
  }

  // - Write the dot file -
  std::string delim;
  os_ << "digraph finite_state_machine {\n"
         " rankdir=LR;\n"
         " subgraph cluster_states {\n"
         " peripheries=0;\n"
         "  node [shape=doublecircle, color=blue]; ";

  for (size_t i = 0; i < state_nrs.size(); ++i) {
    Interval<State::StateNrType> const& interval = state_nrs.get_by_index(i);
    for (State::StateNrType state_nr = interval.get_lower(); state_nr <= interval.get_upper(); ++state_nr) {
      if (state_machine_.get_state(state_nr)->get_accepts().popcnt() != 0) {
        os_ << std::exchange(delim, " ") << state_nr;
      }
    }
  }

  if (!delim.empty()) {
    os_ << ";\n";
  }
  os_ << "  node [shape=circle, color=black];\n";

  // Epsilon transitions
  os_ << "  edge [color=green];\n";
  for (size_t i = 0; i < state_nrs.size(); ++i) {
    Interval<State::StateNrType> const& state_nr_interval = state_nrs.get_by_index(i);
    for (State::StateNrType state_nr = state_nr_interval.get_lower(); state_nr <= state_nr_interval.get_upper(); ++state_nr) {
      IntervalSet<State::StateNrType> const& epsilon_transitions = state_machine_.get_state(state_nr)->get_epsilon_transitions();
      for (size_t j = 0; j < epsilon_transitions.size(); ++j) {
        Interval<State::StateNrType> const& epsilon_interval = epsilon_transitions.get_by_index(j);
        for (State::StateNrType state_nr_next = epsilon_interval.get_lower(); state_nr_next <= epsilon_interval.get_upper(); ++state_nr_next) {
          os_ << "  " << state_nr << " -> " << state_nr_next << "\n";
        }
      }
    }
  }

    // Symbol transitions
    os_ << "  edge [color=black];\n";
    for (auto const& [state_nr, state_nr_next_and_label] : symbol_arrow_labels) {
      for (auto const& [state_nr_next, label] : state_nr_next_and_label) {
        os_ << "  " << state_nr << " -> " << state_nr_next << " [label=\"" << label << "\"]\n";
      }
    }

    os_ << " }\n";

    // Accepts
    std::unordered_map<size_t, std::set<State::StateNrType>> accepts;
    for (size_t i = 0; i < state_nrs.size(); ++i) {
      Interval<State::StateNrType> const& interval = state_nrs.get_by_index(i);
      for (State::StateNrType state_nr = interval.get_lower(); state_nr <= interval.get_upper(); ++state_nr) {
        State const& state = *state_machine_.get_state(state_nr);
        std::set<size_t> const accepts_set = pmt::base::BitsetConverter::to_set(state.get_accepts());
        for (size_t accept : accepts_set) {
          accepts[accept].insert(state_nr);
        }
      }
    }

    os_ << "\n"
           " subgraph cluster_accepts {\n"
           "  label=\"Accepts\";\n"
           "  color=gray;\n"
           "  style=filled;\n\n"
           "  node [shape=record;style=filled;fillcolor=white];\n\n";

    os_ << "  node_accepts [label=\"";
    delim.clear();
    for (auto const& [accept, state_nrs] : accepts) {
      std::string const lhs = accepts_to_label_(accept);
      os_ << std::exchange(delim, "|") << lhs << ": ";

      std::string delim2;
      for (State::StateNrType state_nr : state_nrs) {
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

  auto GraphWriter::create_label(pmt::base::IntervalSet<Symbol::UnderlyingType> const& symbols_) -> std::string {
    std::vector<std::pair<Symbol::UnderlyingType, Symbol::UnderlyingType>> ranges;

    // todo: this is a bit lazy
    std::set<Symbol::UnderlyingType> symbols;
    {
      for (size_t i = 0; i < symbols_.size(); ++i) {
        Interval<Symbol::UnderlyingType> const& symbol_interval = symbols_.get_by_index(i);
        for (Symbol::UnderlyingType c = symbol_interval.get_lower(); c <= symbol_interval.get_upper(); ++c) {
          symbols.insert(c);
        }
      }
    }

    for (auto itr_cur = symbols.begin(); itr_cur != symbols.end(); ++itr_cur) {
      Symbol::UnderlyingType const symbol_first = *itr_cur;
      Symbol::UnderlyingType symbol_cur = *itr_cur;
      bool const displayale_cur = is_displayable(symbol_cur);

      while (true) {
        auto itr_next = std::next(itr_cur);
        if (itr_next == symbols.end()) {
          break;
        }

        Symbol::UnderlyingType const symbol_next = *itr_next;

        if (symbol_next != symbol_cur + 1 || is_displayable(symbol_next) != displayale_cur) {
          break;
        }

        itr_cur = itr_next;
        symbol_cur = symbol_next;
      }

      ranges.emplace_back(symbol_first, symbol_cur);
    }

    std::string ret;
    std::string delim;
    for (size_t i = 0; i < ranges.size(); ++i) {
      auto const& range_info = ranges[i];
      ret += std::exchange(delim, ", ");
      if (range_info.first == range_info.second) {
        ret += to_displayable(range_info.first);
      } else {
        ret += "[" + to_displayable(range_info.first) + ".." + to_displayable(range_info.second) + "]";
      }
    }

    return ret;
  }

  auto GraphWriter::is_displayable(Symbol::UnderlyingType symbol_) -> bool {
    static std::unordered_set<Symbol::UnderlyingType> const DISPLAYABLES = {
     // clang-format off
      '!', '#', '$', '%', '&', '(', ')', '*', ',', '-', '.', '@', '^', '_',
      '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
      'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
      'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',
     // clang-format on
    };

    return DISPLAYABLES.contains(symbol_);
  }

  auto GraphWriter::to_displayable(Symbol::UnderlyingType symbol_) -> std::string {
    if (is_displayable(symbol_)) {
      return R"(')" + std::string(1, symbol_) + R"(')";
    }

    std::stringstream ss;
    ss << std::hex << symbol_;
    std::string ret = ss.str();
    std::transform(ret.begin(), ret.end(), ret.begin(), [](auto const c_) { return std::toupper(c_); });
    return "0x" + ret;
  }

}  // namespace pmt::util::smct