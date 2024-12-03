#include "pmt/util/parse/graph_writer.hpp"

#include "pmt/base/dynamic_bitset.hpp"

#include <iostream>
#include <map>
#include <unordered_map>
#include <utility>

namespace pmt::util::parse {

void GraphWriter::write_dot(std::ostream& os_, Fa const& fa_, AcceptsToLabel accepts_to_label_, SymbolToLabel symbol_to_label_) {
  if (!accepts_to_label_) {
    accepts_to_label_ = accepts_to_label_default;
  }

  if (!symbol_to_label_) {
    symbol_to_label_ = symbol_to_label_default;
  }

  // - Build up the info -
  std::unordered_map<size_t, std::unordered_map<size_t, std::string>> symbol_arrows;
  std::unordered_map<size_t, std::unordered_set<size_t>> epsilon_arrows;
  std::map<size_t, pmt::base::DynamicBitset> accepts;

  for (auto const& [state_nr, state] : fa_._states) {
    // Accepts
    if (state._accepts.popcnt() != 0) {
      accepts.insert_or_assign(state_nr, state._accepts);
    }

    // Epislon transitions
    {
      std::unordered_set<size_t>& epsilon_arrows_state = epsilon_arrows[state_nr];
      for (size_t state_nr_next : state._transitions._epsilon_transitions) {
        epsilon_arrows_state.insert(state_nr_next);
      }
    }

    // Symbol transitions
    if (state._transitions._symbol_transitions.empty()) {
      continue;
    }

    {
      std::unordered_map<size_t, std::string>& symbol_arrows_state = symbol_arrows[state_nr];
      for (auto const& [symbol, state_nr_next] : state._transitions._symbol_transitions) {
        std::string& label = symbol_arrows_state[state_nr_next];
        label += label.empty() ? "" : " ";
        label += symbol_to_label_(symbol);
      }
    }
  }

  // - Write the dot file -
  std::string delim;
  os_ << "digraph finite_state_machine {\n"
         " rankdir=LR;\n"
         " node [shape=doublecircle, color=blue]; ";
  for (auto const& [state_nr, state] : accepts) {
    os_ << std::exchange(delim, " ") << state_nr;
  }
  if (!delim.empty()) {
    os_ << ";\n";
  }
  os_ << " node [shape=circle, color=black];\n";
  // Epsilon transitions
  os_ << " edge [color=green];\n";

  for (auto const& [state_nr, state_nr_nexts] : epsilon_arrows) {
    for (size_t state_nr_next : state_nr_nexts) {
      os_ << " " << state_nr << " -> " << state_nr_next << "\n";
    }
  }

  // Symbol transitions
  os_ << " edge [color=black];\n";
  for (auto const& [state_nr, symbol_arrows_state] : symbol_arrows) {
    for (auto [state_nr_next, label] : symbol_arrows_state) {
      std::sort(label.begin(), label.end());
      os_ << " " << state_nr << " -> " << state_nr_next << " [label=\"" << label << "\"]\n";
    }
  }

  os_ << "}\n";
}

auto GraphWriter::accepts_to_label_default(size_t accepts_) -> std::string {
  return std::to_string(accepts_);
}

auto GraphWriter::symbol_to_label_default(Fa::SymbolType symbol_) -> std::string {
  char const c = static_cast<char>(symbol_);
  if (('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z') || ('0' <= c && c <= '9') || c == '_') {
    return std::string(1, c);
  }
  return std::to_string(symbol_);
}

}  // namespace pmt::util::parse