#pragma once

#include "pmt/util/text_encoding.hpp"

#include <unordered_map>
#include <unordered_set>

namespace pmt::util::parse {

class FaTransitions {
  std::unordered_map<CodepointType, size_t> _symbol_transitions;
  std::unordered_set<size_t> _epsilon_transitions;
};

class FaState {
 public:
  FaTransitions _transitions;
  std::unordered_set<size_t> _accepts;
};

class Fa {
 public:
  std::unordered_map<size_t, FaState> _states;

  void prune(size_t from_ = 0);
  void determinize();
  void minimize();

  auto get_unused_state_nr() const -> size_t;

 private:
  auto get_e_closure(size_t state_nr_) const -> std::unordered_set<size_t>;
  auto get_e_closure(std::unordered_set<size_t> const& state_nrs_) const -> std::unordered_set<size_t>;
};

}  // namespace pmt::util::parse