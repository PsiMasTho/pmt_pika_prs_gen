#pragma once

#include "pmt/base/dynamic_bitset.hpp"
#include "pmt/util/parsect/alphabet_limits.hpp"
#include "pmt/util/parsect/state_machine.hpp"
#include "pmt/util/parsect/state_machine_sink_wrapper.hpp"
#include "pmt/util/parsert/transition_tables.hpp"

#include <vector>

namespace pmt::parserbuilder {

template <pmt::util::parsect::IsStateTag TAG_>
class StateMachineToTransitionTables {
  class Context;

 public:
  static auto convert(pmt::util::parsect::StateMachine const& state_machine_, pmt::util::parsect::AlphabetLimits<TAG_> alphabet_limits_, size_t symbol_kind_) -> pmt::util::parsert::TransitionTables;

 private:
  static void step_1(Context& context_);
  static void step_2(Context& context_);
  static void step_3(Context& context_);
  static void step_4(Context& context_);
  static void step_5(Context& context_);

  static auto debug_post_checks(Context const& context_) -> bool;

  static auto index_2d_to_1d(size_t x_, size_t y_, size_t width_) -> size_t;
  static auto index_1d_to_2d(size_t i_, size_t width_) -> std::pair<size_t, size_t>;
};

template <pmt::util::parsect::IsStateTag TAG_>
class StateMachineToTransitionTables<TAG_>::Context {
 public:
  Context(pmt::util::parsect::StateMachine const& state_machine_, pmt::util::parsect::AlphabetLimits<TAG_> alphabet_limits_, size_t symbol_kind_);

  std::pair<pmt::util::parsect::Symbol::ValueType, pmt::util::parsect::Symbol::ValueType> _alphabet_limits;
  pmt::util::parsect::StateMachineSinkWrapper<TAG_> _state_machine_with_sink;
  std::vector<pmt::util::parsect::State::StateNrType> _ordering;
  std::vector<pmt::base::DynamicBitset> _diff_mat_2d;
  pmt::util::parsert::TransitionTables _tables;
  size_t _symbol_kind;
};

}  // namespace pmt::parserbuilder

#include "pmt/parserbuilder/state_machine_to_transition_tables-inl.hpp"