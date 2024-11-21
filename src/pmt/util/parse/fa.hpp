#pragma once

#include "pmt/base/dynamic_bitset.hpp"
#include "pmt/util/text_encoding.hpp"

#include <unordered_map>
#include <unordered_set>

namespace pmt::util::parse {

class FaTransitions {
 public:
  std::unordered_map<size_t, size_t> _symbol_transitions;
  std::unordered_set<size_t> _epsilon_transitions;
};

class FaState {
 public:
  FaTransitions _transitions;
  pmt::base::DynamicBitset _accepts;
};

using GenericTransitions = std::unordered_map<size_t, std::unordered_set<size_t>>;

class Fa {
 public:
  std::unordered_map<size_t, FaState> _states;

  void prune(size_t from_ = 0);
  void determinize();
  void minimize();

  auto get_unused_state_nr() const -> size_t;
  auto get_backward_transitions() -> GenericTransitions;
  auto get_forward_transitions() -> GenericTransitions;

  static auto get_reachable_state_nrs(std::unordered_set<size_t> from_) -> std::unordered_set<size_t>;

 private:
  auto get_e_closure(std::unordered_set<size_t> const& state_nrs_) const -> std::unordered_set<size_t>;
  auto get_moves(std::unordered_set<size_t> const& state_nrs_, CodepointType symbol_) const -> std::unordered_set<size_t>;
};

}  // namespace pmt::util::parse