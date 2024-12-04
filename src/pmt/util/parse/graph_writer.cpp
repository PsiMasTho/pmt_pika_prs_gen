#include "pmt/util/parse/graph_writer.hpp"

#include "pmt/base/dynamic_bitset.hpp"
#include "pmt/base/dynamic_bitset_converter.hpp"

#include <cstring>
#include <ios>
#include <iostream>
#include <sstream>
#include <unordered_map>
#include <utility>

namespace pmt::util::parse {

void GraphWriter::write_dot(std::ostream& os_, Fa const& fa_, AcceptsToLabel accepts_to_label_) {
  if (!accepts_to_label_) {
    accepts_to_label_ = accepts_to_label_default;
  }

  // <state_nr> -> <state_nr_next, label>
  std::unordered_map<Fa::StateNrType, std::unordered_map<Fa::StateNrType, std::string>> symbol_arrow_labels;

  for (auto const& [state_nr, state] : fa_._states) {
    // Symbol transitions
    if (state._transitions._symbol_transitions.empty()) {
      continue;
    }

    std::unordered_map<Fa::StateNrType, std::set<Fa::SymbolType>> relations;
    for (auto const& [symbol, state] : state._transitions._symbol_transitions) {
      relations[state].insert(symbol);
    }

    for (auto const& [state_nr_next, symbols] : relations) {
      symbol_arrow_labels[state_nr][state_nr_next] = create_label(symbols);
    }
  }

  // - Write the dot file -
  std::string delim;
  os_ << "digraph finite_state_machine {\n"
         " rankdir=LR;\n"
         " node [shape=doublecircle, color=blue]; ";
  for (auto const& [state_nr, state] : fa_._states) {
    if (state._accepts.popcnt() != 0) {
      os_ << std::exchange(delim, " ") << state_nr;
    }
  }
  if (!delim.empty()) {
    os_ << ";\n";
  }
  os_ << " node [shape=circle, color=black];\n";

  // Epsilon transitions
  os_ << " edge [color=green];\n";
  for (auto const& [state_nr, state] : fa_._states) {
    for (Fa::StateNrType state_nr_next : state._transitions._epsilon_transitions) {
      os_ << " " << state_nr << " -> " << state_nr_next << "\n";
    }
  }

  // Symbol transitions
  os_ << " edge [color=black];\n";
  for (auto const& [state_nr, state_nr_next_and_label] : symbol_arrow_labels) {
    for (auto const& [state_nr_next, label] : state_nr_next_and_label) {
      os_ << " " << state_nr << " -> " << state_nr_next << " [label=\"" << label << "\"]\n";
    }
  }

  // Accepts
  std::unordered_map<size_t, std::set<Fa::StateNrType>> accepts;
  for (auto const& [state_nr, state] : fa_._states) {
    std::set<size_t> const accepts_set = pmt::base::DynamicBitsetConverter::to_set(state._accepts);
    for (size_t accept : accepts_set) {
      accepts[accept].insert(state_nr);
    }
  }

  os_ << "\n"
         " subgraph cluster_accepts {\n"
         " label=\"Accepts\";\n"
         " color=gray;\n"
         " style=filled;\n\n"
         " node [shape=record;style=filled;fillcolor=white];\n\n";

  os_ << " node_accepts [label=\"";
  delim.clear();
  for (auto const& [accept, state_nrs] : accepts) {
    std::string const lhs = accepts_to_label_(accept);
    os_ << std::exchange(delim, "|") << lhs << ": ";

    std::string delim2;
    for (Fa::StateNrType state_nr : state_nrs) {
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

auto GraphWriter::create_label(std::set<Fa::SymbolType> const& state_nrs_) -> std::string {
  std::vector<std::pair<Fa::SymbolType, Fa::SymbolType>> ranges;

  for (auto itr_cur = state_nrs_.begin(); itr_cur != state_nrs_.end(); ++itr_cur) {
    Fa::SymbolType const sym_first = *itr_cur;
    Fa::SymbolType sym_cur = *itr_cur;
    bool const displayale_cur = is_displayable(sym_cur);

    while (true) {
      auto itr_next = std::next(itr_cur);
      if (itr_next == state_nrs_.end()) {
        break;
      }

      Fa::SymbolType const sym_next = *itr_next;

      if (sym_next != sym_cur + 1 || is_displayable(sym_next) != displayale_cur) {
        break;
      }

      itr_cur = itr_next;
      sym_cur = sym_next;
    }

    ranges.emplace_back(sym_first, sym_cur);
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

auto GraphWriter::is_displayable(Fa::SymbolType sym_) -> bool {
  return ('a' <= sym_ && sym_ <= 'z') || ('A' <= sym_ && sym_ <= 'Z') || ('0' <= sym_ && sym_ <= '9') || (std::strchr("!#$%&()*,-..@^_", sym_) != nullptr);
}

auto GraphWriter::to_displayable(Fa::SymbolType sym_) -> std::string {
  if (is_displayable(sym_)) {
    return R"(')" + std::string(1, sym_) + R"(')";
  }

  std::stringstream ss;
  ss << std::hex << sym_;
  std::string ret = ss.str();
  std::transform(ret.begin(), ret.end(), ret.begin(), [](auto const c) { return std::toupper(c); });
  return "0x" + ret;
}

}  // namespace pmt::util::parse