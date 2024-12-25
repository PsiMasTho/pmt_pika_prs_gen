#include "pmt/parserbuilder/fa_to_fa_state_tables.hpp"

#include "pmt/util/parsert/generic_tables_base.hpp"

#include <algorithm>
#include <cassert>
#include <cstdint>
#include <iterator>
#include <limits>
#include <utility>

namespace pmt::parserbuilder {
using namespace pmt::util::parsect;
using namespace pmt::util::parsert;
using namespace pmt::base;

namespace {
auto index_2d_to_1d(size_t x_, size_t y_, size_t width_) -> size_t {
  return y_ * width_ + x_;
}

auto index_1d_to_2d(size_t i_, size_t width_) -> std::pair<size_t, size_t> {
  return {i_ % width_, i_ / width_};
}
}  // namespace

void FaToFaStateTables::convert(pmt::util::parsect::Fa const& fa_, pmt::util::parsert::GenericFaStateTables& fa_state_tables_) {
  Context context(fa_, fa_state_tables_);

  step_1(context);
  step_2(context);
  step_3(context);
  step_4(context);
  step_5(context);

  debug_post_checks(context);
}

void FaToFaStateTables::step_1(Context& context_) {
  size_t const width = context_._fa_with_sink.get_size();
  context_._diff_mat_2d.reserve(width * width);

  for (size_t i = 0; i < width * width; ++i) {
    auto const [j1, j2] = index_1d_to_2d(i, width);

    context_._diff_mat_2d.emplace_back(GenericTablesBase::SYMBOL_EOI + 1, false);
    for (Fa::SymbolType k = 0; k <= GenericTablesBase::SYMBOL_EOI; ++k) {
      context_._diff_mat_2d.back().set(k, context_._fa_with_sink.get_state_nr_next(j1, k) != context_._fa_with_sink.get_state_nr_next(j2, k));
    }
  }
}

void FaToFaStateTables::step_2(Context& context_) {
  std::unordered_set<Fa::StateNrType> unvisited;
  std::generate_n(std::inserter(unvisited, unvisited.end()), context_._fa_with_sink.get_size(), [i = 0]() mutable { return i++; });
  context_._ordering.reserve(context_._fa_with_sink.get_size());

  // Fill the ordering based on the lowest average differences to other states
  while (!unvisited.empty()) {
    double avg_diff_best = std::numeric_limits<double>::max();
    Fa::StateNrType state_nr_best = std::numeric_limits<Fa::StateNrType>::max();

    for (Fa::StateNrType i : unvisited) {
      double avg_diff = 0.f;
      for (Fa::StateNrType j : unvisited) {
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
  if (std::optional<Fa::StateNrType> const state_nr_sink = context_._fa_with_sink.get_state_nr_sink()) {
    auto const itr = std::find(context_._ordering.begin(), context_._ordering.end(), *state_nr_sink);
    std::move_backward(context_._ordering.begin(), itr, itr + 1);
    context_._ordering.front() = *state_nr_sink;
  }

  context_._tables._state_nr_sink = context_._ordering.front();
}

void FaToFaStateTables::step_3(Context& context_) {
  context_._tables._state_transition_entries.resize(context_._fa_with_sink.get_size());
  context_._tables._state_transition_entries[context_._ordering.front()]._default = context_._ordering.front();

  // For every state, pick the most similar state that appears before it in the ordering
  // as the default for that state
  for (size_t i = 0; i < context_._ordering.size(); ++i) {
    Fa::StateNrType const state_nr_i = context_._ordering[i];

    Fa::StateNrType state_nr_best = context_._ordering.front();
    size_t popcnt_best = context_._diff_mat_2d[index_2d_to_1d(state_nr_i, state_nr_best, context_._fa_with_sink.get_size())].popcnt();

    for (Fa::StateNrType j = 0; j < i; ++j) {
      Fa::StateNrType const state_nr_j = context_._ordering[j];
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

void FaToFaStateTables::step_4(Context& context_) {
  std::unordered_map<Fa::StateNrType, uint64_t> shifts;
  std::unordered_map<size_t, std::pair<Fa::StateNrType, Fa::StateNrType>> occupied;  // <index, which state filled it, state_nr_to>
  size_t shift_max = 0;
  for (Fa::StateNrType state_nr_i : context_._ordering) {
    DynamicBitset const& keep = context_._diff_mat_2d[index_2d_to_1d(state_nr_i, context_._tables._state_transition_entries[state_nr_i]._default, context_._fa_with_sink.get_size())];
    std::unordered_map<size_t, Fa::StateNrType> kept;
    for (Fa::SymbolType j = 0; j < UCHAR_MAX; ++j) {
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

  context_._tables._compressed_transition_entries.resize(shift_max + GenericTablesBase::SYMBOL_EOI + 1, {context_._tables._state_nr_sink, context_._tables._state_nr_sink});

  for (auto const& [i, shift] : shifts) {
    context_._tables._state_transition_entries[i]._shift = shift;
  }

  for (auto const& [i, j] : occupied) {
    auto const& [check, next] = j;
    context_._tables._compressed_transition_entries[i] = {check, next};
  }
}

void FaToFaStateTables::step_5(Context& context_) {
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
}

auto FaToFaStateTables::debug_post_checks(Context const& context_) -> bool {
#ifndef NDEBUG
  // check that transitions in the tables match the transitions in the fa
  for (size_t i = 0; i < context_._fa_with_sink.get_size(); ++i) {
    for (Fa::SymbolType symbol = 0; symbol < UCHAR_MAX; ++symbol) {
      Fa::StateNrType const state_nr_next_fa = context_._fa_with_sink.get_state_nr_next(i, symbol);
      Fa::StateNrType const state_nr_next = context_._tables.get_state_nr_next(i, symbol);
      if (state_nr_next != state_nr_next_fa) {
        throw std::runtime_error("Transition mismatch for state " + std::to_string(i) + " and symbol " + std::to_string(symbol) + ": " + std::to_string(state_nr_next) + " != " + std::to_string(state_nr_next_fa));
      }
    }
  }

#endif
  return true;
}

FaToFaStateTables::Context::Context(pmt::util::parsect::Fa const& fa_, pmt::util::parsert::GenericFaStateTables& fa_state_tables_)
 : _fa_with_sink(fa_)
 , _tables(fa_state_tables_) {
}

}  // namespace pmt::parserbuilder