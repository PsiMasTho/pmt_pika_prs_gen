// clang-format off
#ifdef __INTELLISENSE__
 #include "pmt/parserbuilder/state_machine_to_transition_tables.hpp"
#endif
// clang-format on

#include <algorithm>
#include <cassert>
#include <cstdint>
#include <iterator>
#include <limits>
#include <utility>

namespace pmt::parserbuilder {

template <pmt::util::smct::IsStateTag TAG_>
auto StateMachineToTransitionTables<TAG_>::convert(pmt::util::smct::StateMachine const& state_machine_, pmt::util::smct::AlphabetLimits<TAG_> alphabet_limits_, size_t symbol_kind_) -> pmt::util::smrt::TransitionTables {
  Context context(state_machine_, alphabet_limits_, symbol_kind_);

  std::optional<std::pair<pmt::util::smct::Symbol::ValueType, pmt::util::smct::Symbol::ValueType>> const limits = context._alphabet_limits.get_limit(context._symbol_kind);
  if (!limits.has_value()) {
    return context._tables;
  }

  context._alphabet_limits = *limits;

  step_1(context);
  step_2(context);
  step_3(context);
  step_4(context);
  step_5(context);

  debug_post_checks(context);

  return context._tables;
}

template <pmt::util::smct::IsStateTag TAG_>
void StateMachineToTransitionTables<TAG_>::step_1(Context& context_) {
  size_t const width = context_._fa_with_sink.get_size();
  context_._diff_mat_2d.reserve(width * width);

  for (size_t i = 0; i < width * width; ++i) {
    auto const [j1, j2] = index_1d_to_2d(i, width);

    context_._diff_mat_2d.emplace_back(context_._alphabet_limits.second - context_._alphabet_limits.first + 1, false);
    for (pmt::util::smct::Symbol::ValueType k = context_._alphabet_limits.first; k <= context_._alphabet_limits.second; ++k) {
      context_._diff_mat_2d.back().set(k, context_._fa_with_sink.get_state_nr_next(j1, k) != context_._fa_with_sink.get_state_nr_next(j2, k));
    }
  }
}

template <pmt::util::smct::IsStateTag TAG_>
void StateMachineToTransitionTables<TAG_>::step_2(Context& context_) {
  std::unordered_set<pmt::util::smct::State::StateNrType> unvisited;
  std::generate_n(std::inserter(unvisited, unvisited.end()), context_._fa_with_sink.get_size(), [i = 0]() mutable { return i++; });
  context_._ordering.reserve(context_._fa_with_sink.get_size());

  // Fill the ordering based on the lowest average differences to other states
  while (!unvisited.empty()) {
    double avg_diff_best = std::numeric_limits<double>::max();
    pmt::util::smct::State::StateNrType state_nr_best = std::numeric_limits<pmt::util::smct::State::StateNrType>::max();

    for (pmt::util::smct::State::StateNrType i : unvisited) {
      double avg_diff = 0.f;
      for (pmt::util::smct::State::StateNrType j : unvisited) {
        if (i == j) {
          continue;
        }
        avg_diff += context_._diff_mat_2d[index_2d_to_1d(i, j, context_._fa_with_sink.get_size())].popcnt();
      }
      avg_diff /= unvisited.size();

      if (avg_diff < avg_diff_best) {
        avg_diff_best = avg_diff;
        state_nr_best = i;
      }
    }

    context_._ordering.push_back(state_nr_best);
    unvisited.erase(state_nr_best);
  }

  // Move the sink state to the front
  if (std::optional<pmt::util::smct::State::StateNrType> const state_nr_sink = context_._fa_with_sink.get_state_nr_sink()) {
    auto const itr = std::find(context_._ordering.begin(), context_._ordering.end(), *state_nr_sink);
    std::move_backward(context_._ordering.begin(), itr, itr + 1);
    context_._ordering.front() = *state_nr_sink;
  }

  context_._tables._state_nr_sink = context_._ordering.front();
}

template <pmt::util::smct::IsStateTag TAG_>
void StateMachineToTransitionTables<TAG_>::step_3(Context& context_) {
  context_._tables._state_transition_entries.resize(context_._fa_with_sink.get_size());
  context_._tables._state_transition_entries[context_._ordering.front()]._default = context_._ordering.front();

  // For every state, pick the most similar state that appears before it in the ordering
  // as the default for that state
  for (size_t i = 0; i < context_._ordering.size(); ++i) {
    pmt::util::smct::State::StateNrType const state_nr_i = context_._ordering[i];

    pmt::util::smct::State::StateNrType state_nr_best = context_._ordering.front();
    size_t popcnt_best = context_._diff_mat_2d[index_2d_to_1d(state_nr_i, state_nr_best, context_._fa_with_sink.get_size())].popcnt();

    for (pmt::util::smct::State::StateNrType j = 0; j < i; ++j) {
      pmt::util::smct::State::StateNrType const state_nr_j = context_._ordering[j];
      size_t const popcnt_cur = context_._diff_mat_2d[index_2d_to_1d(state_nr_i, state_nr_j, context_._fa_with_sink.get_size())].popcnt();
      if (popcnt_cur < popcnt_best) {
        state_nr_best = state_nr_j;
        popcnt_best = popcnt_cur;
      }
      if (popcnt_cur == 0) {
        break;
      }
    }

    context_._tables._state_transition_entries[state_nr_i]._default = state_nr_best;
  }
}

template <pmt::util::smct::IsStateTag TAG_>
void StateMachineToTransitionTables<TAG_>::step_4(Context& context_) {
  std::unordered_map<pmt::util::smct::State::StateNrType, uint64_t> shifts;
  std::unordered_map<size_t, std::pair<pmt::util::smct::State::StateNrType, pmt::util::smct::State::StateNrType>> occupied;  // <index, which state filled it, state_nr_to>
  size_t shift_max = 0;
  for (pmt::util::smct::State::StateNrType state_nr_i : context_._ordering) {
    pmt::base::Bitset const& keep = context_._diff_mat_2d[index_2d_to_1d(state_nr_i, context_._tables._state_transition_entries[state_nr_i]._default, context_._fa_with_sink.get_size())];
    std::unordered_map<size_t, pmt::util::smct::State::StateNrType> kept;
    for (size_t j = context_._alphabet_limits.first; j <= context_._alphabet_limits.second; ++j) {
      if (!keep.get(j)) {
        continue;
      }
      kept[j] = context_._fa_with_sink.get_state_nr_next(state_nr_i, j);
    }

    // try increasing shift values and use the first one that works
    size_t shift_i = 0;
    while (true) {
      bool good = true;
      for (auto const& [unshifted, state_nr_next] : kept) {
        if (occupied.contains(unshifted + shift_i)) {
          good = false;
          break;
        }
      }

      if (good) {
        break;
      }
      ++shift_i;
    }
    shifts[state_nr_i] = shift_i;
    shift_max = std::max(shift_max, shift_i);
    for (auto const& [unshifted, state_nr_next] : kept) {
      occupied[unshifted + shift_i] = std::make_pair(state_nr_i, state_nr_next);
    }
  }

  context_._tables._compressed_transition_entries.resize(shift_max + context_._alphabet_limits.second - context_._alphabet_limits.first + 1, {context_._tables._state_nr_sink, context_._tables._state_nr_sink});

  for (auto const& [i, shift] : shifts) {
    context_._tables._state_transition_entries[i]._shift = shift;
  }

  for (auto const& [i, j] : occupied) {
    auto const& [check, next] = j;
    context_._tables._compressed_transition_entries[i] = {check, next};
  }
}

template <pmt::util::smct::IsStateTag TAG_>
void StateMachineToTransitionTables<TAG_>::step_5(Context& context_) {
  // Calculate padding
  // Left:
  context_._tables._padding_l = 0;
  while (context_._tables._padding_l < context_._tables._compressed_transition_entries.size()) {
    auto const& [check, next] = context_._tables._compressed_transition_entries[context_._tables._padding_l];
    if (next != context_._tables._state_nr_sink || next != check) {
      break;
    }
    ++context_._tables._padding_l;
  }

  // Right:
  size_t padding_r = 0;
  while (padding_r < context_._tables._compressed_transition_entries.size()) {
    auto const& [check, next] = context_._tables._compressed_transition_entries[context_._tables._compressed_transition_entries.size() - padding_r - 1];
    if (next != context_._tables._state_nr_sink || next != check) {
      break;
    }
    ++padding_r;
  }

  // Erase according to padding
  // Left:
  context_._tables._compressed_transition_entries.erase(context_._tables._compressed_transition_entries.begin(), context_._tables._compressed_transition_entries.begin() + context_._tables._padding_l);

  // Right:
  context_._tables._compressed_transition_entries.resize(context_._tables._compressed_transition_entries.size() - padding_r);

  // Set limits
  context_._tables._symbol_min = context_._alphabet_limits.first;
  context_._tables._symbol_max = context_._alphabet_limits.second;
}

template <pmt::util::smct::IsStateTag TAG_>
auto StateMachineToTransitionTables<TAG_>::debug_post_checks(Context const& context_) -> bool {
#ifndef NDEBUG
  // check that transitions in the tables match the transitions in the fa
  for (size_t i = 0; i < context_._fa_with_sink.get_size(); ++i) {
    for (size_t j = context_._alphabet_limits.first; j <= context_._alphabet_limits.second; ++j) {
      pmt::util::smct::State::StateNrType const state_nr_next_fa = context_._fa_with_sink.get_state_nr_next(i, j);
      pmt::util::smct::State::StateNrType const state_nr_next = context_._tables.get_state_nr_next(i, j);
      if (state_nr_next != state_nr_next_fa) {
        throw std::runtime_error("Transition mismatch for state " + std::to_string(i) + " and symbol " + std::to_string(j) + ": " + std::to_string(state_nr_next) + " != " + std::to_string(state_nr_next_fa));
      }
    }
  }

#endif
  return true;
}

template <pmt::util::smct::IsStateTag TAG_>
auto StateMachineToTransitionTables<TAG_>::index_2d_to_1d(size_t x_, size_t y_, size_t width_) -> size_t {
  return y_ * width_ + x_;
}

template <pmt::util::smct::IsStateTag TAG_>
auto StateMachineToTransitionTables<TAG_>::index_1d_to_2d(size_t i_, size_t width_) -> std::pair<size_t, size_t> {
  return {i_ % width_, i_ / width_};
}

template <pmt::util::smct::IsStateTag TAG_>
StateMachineToTransitionTables<TAG_>::Context::Context(pmt::util::smct::StateMachine const& state_machine_, pmt::util::smct::AlphabetLimits<TAG_> alphabet_limits_, size_t symbol_kind_)
 : _alphabet_limits(alphabet_limits_)
 , _state_machine_with_sink(state_machine_, alphabet_limits_) {
}

}  // namespace pmt::parserbuilder