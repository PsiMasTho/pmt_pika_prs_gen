#include "pmt/parserbuilder/fa_to_dsnc_transitions.hpp"

#include "pmt/util/parsect/fa.hpp"

#include <algorithm>
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

FaToDsncTransitions::FaToDsncTransitions(Fa const& fa_)
 : _fa(fa_) {
}

auto FaToDsncTransitions::convert() -> Dsnc {
  debug_pre_checks();

  Dsnc dsnc;
  step_0(dsnc);
  step_1(dsnc);
  step_2(dsnc);
  step_3(dsnc);
  step_4(dsnc);
  step_5(dsnc);

  debug_post_checks(dsnc);

  return dsnc;
}

void FaToDsncTransitions::step_0(Dsnc& dsnc_) {
  // Add the sink state
  dsnc_._state_nr_sink = _fa.get_unused_state_nr();
  Fa::Transitions& transitions_sink = _fa._states[dsnc_._state_nr_sink]._transitions;
  for (Fa::SymbolType i = 0; i < UCHAR_MAX; ++i) {
    transitions_sink._symbol_transitions[i] = dsnc_._state_nr_sink;
  }

  for (auto& [state_nr, state] : _fa._states) {
    for (Fa::SymbolType i = 0; i < UCHAR_MAX; ++i) {
      if (!state._transitions._symbol_transitions.contains(i)) {
        state._transitions._symbol_transitions[i] = dsnc_._state_nr_sink;
      }
    }
  }

  dsnc_._state_transition_entries.resize(_fa._states.size());
}

void FaToDsncTransitions::step_1(Dsnc&) {
  size_t const width = _fa._states.size();
  _diff_mat_2d.reserve(width * width);

  for (size_t i = 0; i < width * width; ++i) {
    auto const [j1, j2] = index_1d_to_2d(i, width);
    Fa::Transitions const& transitions_j1 = _fa._states.find(j1)->second._transitions;
    Fa::Transitions const& transitions_j2 = _fa._states.find(j2)->second._transitions;

    _diff_mat_2d.emplace_back(UCHAR_MAX, false);
    for (Fa::SymbolType k = 0; k < UCHAR_MAX; ++k) {
      _diff_mat_2d.back().set(k, transitions_j1._symbol_transitions.find(k)->second != transitions_j2._symbol_transitions.find(k)->second);
    }
  }
}

void FaToDsncTransitions::step_2(Dsnc& dsnc_) {
  std::unordered_set<Fa::StateNrType> unvisited;
  std::generate_n(std::inserter(unvisited, unvisited.end()), _fa._states.size(), [i = 0]() mutable { return i++; });
  _ordering.reserve(_fa._states.size());

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
        avg_diff += _diff_mat_2d[index_2d_to_1d(i, j, _fa._states.size())].popcnt();
      }
      avg_diff /= unvisited.size();

      if (avg_diff < avg_diff_best) {
        avg_diff_best = avg_diff;
        state_nr_best = i;
      }
    }

    _ordering.push_back(state_nr_best);
    unvisited.erase(state_nr_best);
  }

  dsnc_._state_nr_min_diff = _ordering.front();
}

void FaToDsncTransitions::step_3(Dsnc& dsnc_) {
  dsnc_._state_transition_entries[_ordering.front()]._default = _ordering.front();

  // For every state, pick the most similar state that appears before it in the ordering
  // as the default for that state
  for (size_t i = 0; i < _ordering.size(); ++i) {
    Fa::StateNrType const state_nr_i = _ordering[i];

    Fa::StateNrType state_nr_best = _ordering.front();
    size_t popcnt_best = _diff_mat_2d[index_2d_to_1d(state_nr_i, state_nr_best, _fa._states.size())].popcnt();

    for (Fa::StateNrType j = 0; j < i; ++j) {
      Fa::StateNrType const state_nr_j = _ordering[j];
      size_t const popcnt_cur = _diff_mat_2d[index_2d_to_1d(state_nr_i, state_nr_j, _fa._states.size())].popcnt();
      if (popcnt_cur < popcnt_best) {
        state_nr_best = state_nr_j;
        popcnt_best = popcnt_cur;
      }
      if (popcnt_cur == 0) {
        break;
      }
    }

    dsnc_._state_transition_entries[state_nr_i]._default = state_nr_best;
  }
}

void FaToDsncTransitions::step_4(Dsnc& dsnc_) {
  std::unordered_map<Fa::StateNrType, uint64_t> shifts;
  std::unordered_map<size_t, std::pair<Fa::StateNrType, Fa::StateNrType>> occupied;  // <index, which state filled it, state_nr_to>
  size_t shift_max = 0;
  for (Fa::StateNrType state_nr_i : _ordering) {
    DynamicBitset const& keep = _diff_mat_2d[index_2d_to_1d(state_nr_i, dsnc_._state_transition_entries[state_nr_i]._default, _fa._states.size())];
    std::unordered_map<size_t, Fa::StateNrType> kept;
    for (Fa::SymbolType j = 0; j < UCHAR_MAX; ++j) {
      if (!keep.get(j)) {
        continue;
      }
      kept[j] = get_next_state(_fa, state_nr_i, j);
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

  dsnc_._compressed_transition_entries.resize(shift_max + UCHAR_MAX, {dsnc_._state_nr_min_diff, dsnc_._state_nr_min_diff});

  for (auto const& [i, shift] : shifts) {
    dsnc_._state_transition_entries[i]._shift = shift;
  }

  for (auto const& [i, j] : occupied) {
    auto const& [check, next] = j;
    dsnc_._compressed_transition_entries[i] = {next, check};
  }
}

void FaToDsncTransitions::step_5(Dsnc& dsnc_) {
  // Calculate padding
  // Left:
  while (dsnc_._padding_l < dsnc_._compressed_transition_entries.size()) {
    auto const& [next, check] = dsnc_._compressed_transition_entries[dsnc_._padding_l];
    if (next != dsnc_._state_nr_min_diff || next != check) {
      break;
    }
    ++dsnc_._padding_l;
  }

  // Right:
  while (dsnc_._padding_r < dsnc_._compressed_transition_entries.size()) {
    auto const& [next, check] = dsnc_._compressed_transition_entries[dsnc_._compressed_transition_entries.size() - dsnc_._padding_r - 1];
    if (next != dsnc_._state_nr_min_diff || next != check) {
      break;
    }
    ++dsnc_._padding_r;
  }

  // Erase according to padding
  // Left:
  dsnc_._compressed_transition_entries.erase(dsnc_._compressed_transition_entries.begin(), dsnc_._compressed_transition_entries.begin() + dsnc_._padding_l);

  // Right:
  dsnc_._compressed_transition_entries.resize(dsnc_._compressed_transition_entries.size() - dsnc_._padding_r);
}

auto FaToDsncTransitions::debug_pre_checks() const -> bool {
#ifndef NDEBUG
  // Check that state numbers start from 0 and are contiguous
  for (Fa::StateNrType i = 0; i < _fa._states.size(); ++i) {
    if (_fa._states.find(i) == _fa._states.end()) {
      throw std::runtime_error("State number " + std::to_string(i) + " is missing");
    }
  }
#endif
  return true;
}

auto FaToDsncTransitions::debug_post_checks(Dsnc const& dsnc_) const -> bool {
#ifndef NDEBUG
  /*
    std::cout << "-- Post checks --\n";
    std::cout << "FA State count: " << _fa._states.size() << '\n';
    std::cout << "DSNC::sink: " << dsnc_._state_nr_sink << '\n';
    std::cout << "DSNC::min diff: " << dsnc_._state_nr_min_diff << '\n';
    std::cout << "DSNC::lpadding: " << dsnc_._padding_l << '\n';
    std::cout << "DSNC::rpadding: " << dsnc_._padding_r << '\n';
    std::cout << "DSNC::default size: " << dsnc_._transitions_default.size() << '\n';
    std::cout << "DSNC::shift size: " << dsnc_._transitions_shift.size() << '\n';
    std::cout << "DSNC::next size: " << dsnc_._transitions_next.size() << '\n';
    std::cout << "DSNC::check size: " << dsnc_._transitions_check.size() << '\n';
  */

  GenericLexerTables tables;
  tables._padding_l = dsnc_._padding_l;
  tables._padding_r = dsnc_._padding_r;
  tables._state_nr_sink = dsnc_._state_nr_sink;
  tables._state_nr_min_diff = dsnc_._state_nr_min_diff;
  tables._state_transition_entries = dsnc_._state_transition_entries.data();
  tables._compressed_transition_entries = dsnc_._compressed_transition_entries.data();

  // check that transitions in the tables match the transitions in the fa
  for (size_t i = 0; i < _fa._states.size(); ++i) {
    for (Fa::SymbolType symbol = 0; symbol < UCHAR_MAX; ++symbol) {
      Fa::StateNrType const state_nr_next_fa = get_next_state(_fa, i, symbol);
      Fa::StateNrType const state_nr_next = tables.get_next_state(i, symbol);
      if (state_nr_next != state_nr_next_fa) {
        throw std::runtime_error("Transition mismatch for state " + std::to_string(i) + " and symbol " + std::to_string(symbol) + ": " + std::to_string(state_nr_next) + " != " + std::to_string(state_nr_next_fa));
      }
    }
  }

#endif
  return true;
}

auto FaToDsncTransitions::get_next_state(pmt::util::parsect::Fa const& fa_, pmt::util::parsect::Fa::StateNrType state_nr_, pmt::util::parsect::Fa::SymbolType symbol_) -> pmt::util::parsect::Fa::StateNrType {
  Fa::Transitions const& transitions = fa_._states.find(state_nr_)->second._transitions;
  auto const itr = transitions._symbol_transitions.find(symbol_);
  return itr != transitions._symbol_transitions.end() ? itr->second : fa_._states.size();
}

}  // namespace pmt::parserbuilder