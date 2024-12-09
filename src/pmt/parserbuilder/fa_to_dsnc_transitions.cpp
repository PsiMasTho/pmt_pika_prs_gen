#include "pmt/parserbuilder/fa_to_dsnc_transitions.hpp"

#include "pmt/base/dynamic_bitset.hpp"
#include "pmt/util/parse/fa.hpp"

#include <algorithm>
#include <cstdint>
#include <iterator>
#include <limits>
#include <utility>

namespace pmt::parserbuilder {
using namespace pmt::util::parse;
using namespace pmt::base;

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
}

void FaToDsncTransitions::step_1(Dsnc& dsnc_) {
  std::generate_n(std::back_inserter(_ordering), _fa._states.size(), [i = 0]() mutable { return i++; });

  std::vector<size_t> frequencies(_fa._states.size(), 0);
  for (auto const& [state_nr, state] : _fa._states) {
    for (auto const& [symbol, state_nr_next] : state._transitions._symbol_transitions) {
      ++frequencies[state_nr_next];
    }
  }

  std::ranges::sort(_ordering, [&frequencies](size_t lhs_, size_t rhs_) { return frequencies[lhs_] > frequencies[rhs_]; });

  // Determine the most frequent state
  dsnc_._state_nr_most_frequent = _ordering.front();
}

void FaToDsncTransitions::step_2(Dsnc& dsnc_) {
  dsnc_._transitions_default.resize(_fa._states.size(), std::numeric_limits<uint64_t>::max());
  _keep.resize(_fa._states.size());

  dsnc_._transitions_default[_ordering.front()] = _ordering.front();
  _keep[_ordering.front()] = DynamicBitset(UCHAR_MAX, true);

  // For every state, pick the most similar state that appears before it in the ordering,
  // and keep track of the symbols that differ between the two states
  for (size_t i = 1; i < _ordering.size(); ++i) {
    Fa::StateNrType const state_nr_i = _ordering[i];

    Fa::StateNrType state_nr_best = _ordering.front();
    DynamicBitset keep_best = _keep[state_nr_best];

    for (Fa::StateNrType j = 0; j < i; ++j) {
      Fa::StateNrType const state_nr_j = _ordering[j];

      DynamicBitset keep_cur(UCHAR_MAX, false);
      for (Fa::SymbolType k = 0; k < UCHAR_MAX; ++k) {
        keep_cur.set(k, get_next_state(_fa, state_nr_i, k) != get_next_state(_fa, state_nr_j, k));
      }

      size_t const popcnt_cur = keep_cur.popcnt();
      if (popcnt_cur < keep_best.popcnt()) {
        state_nr_best = state_nr_j;
        keep_best = std::move(keep_cur);
      }
      if (popcnt_cur == 0) {
        break;
      }
    }

    dsnc_._transitions_default[state_nr_i] = state_nr_best;
    _keep[state_nr_i] = std::move(keep_best);
  }
}

void FaToDsncTransitions::step_3(Dsnc& dsnc_) {
  std::unordered_map<Fa::StateNrType, uint64_t> shifts;
  std::unordered_map<size_t, std::pair<Fa::StateNrType, Fa::StateNrType>> occupied;  // <index, which state filled it, state_nr_to>
  size_t shift_max = 0;
  for (Fa::StateNrType state_nr_i : _ordering) {
    std::unordered_map<size_t, Fa::StateNrType> kept;
    for (Fa::SymbolType j = 0; j < UCHAR_MAX; ++j) {
      if (!_keep[state_nr_i].get(j)) {
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

  dsnc_._transitions_shift.resize(_fa._states.size(), std::numeric_limits<uint64_t>::max());
  dsnc_._transitions_next.resize(shift_max + UCHAR_MAX, dsnc_._state_nr_most_frequent);
  dsnc_._transitions_check.resize(shift_max + UCHAR_MAX, dsnc_._state_nr_most_frequent);

  for (auto const& [i, shift] : shifts) {
    dsnc_._transitions_shift[i] = shift;
  }

  for (auto const& [i, j] : occupied) {
    auto const& [check, next] = j;
    dsnc_._transitions_next[i] = next;
    dsnc_._transitions_check[i] = check;
  }
}

void FaToDsncTransitions::step_4(Dsnc& dsnc_) {
  // Calculate padding
  // Left:
  while (dsnc_._padding_l < dsnc_._transitions_next.size()) {
    if (dsnc_._transitions_next[dsnc_._padding_l] != dsnc_._state_nr_most_frequent || dsnc_._transitions_check[dsnc_._padding_l] != dsnc_._state_nr_most_frequent) {
      break;
    }
    ++dsnc_._padding_l;
  }

  // Right:
  while (dsnc_._padding_r < dsnc_._transitions_next.size()) {
    if (dsnc_._transitions_next[dsnc_._transitions_next.size() - dsnc_._padding_r - 1] != dsnc_._state_nr_most_frequent || dsnc_._transitions_check[dsnc_._transitions_next.size() - dsnc_._padding_r - 1] != dsnc_._state_nr_most_frequent) {
      break;
    }
    ++dsnc_._padding_r;
  }

  // Erase according to padding
  // Left:
  dsnc_._transitions_next.erase(dsnc_._transitions_next.begin(), dsnc_._transitions_next.begin() + dsnc_._padding_l);
  dsnc_._transitions_check.erase(dsnc_._transitions_check.begin(), dsnc_._transitions_check.begin() + dsnc_._padding_l);

  // Right:
  dsnc_._transitions_next.resize(dsnc_._transitions_next.size() - dsnc_._padding_r);
  dsnc_._transitions_check.resize(dsnc_._transitions_check.size() - dsnc_._padding_r);
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
  std::cout << "DSNC::most frequent: " << dsnc_._state_nr_most_frequent << '\n';
  std::cout << "DSNC::lpadding: " << dsnc_._padding_l << '\n';
  std::cout << "DSNC::rpadding: " << dsnc_._padding_r << '\n';
  std::cout << "DSNC::default size: " << dsnc_._transitions_default.size() << '\n';
  std::cout << "DSNC::shift size: " << dsnc_._transitions_shift.size() << '\n';
  std::cout << "DSNC::next size: " << dsnc_._transitions_next.size() << '\n';
  std::cout << "DSNC::check size: " << dsnc_._transitions_check.size() << '\n';
  */
  // check that transitions in the tables match the transitions in the fa
  for (size_t i = 0; i < _fa._states.size(); ++i) {
    for (Fa::SymbolType symbol = 0; symbol < UCHAR_MAX; ++symbol) {
      Fa::StateNrType const state_nr_next_fa = get_next_state(_fa, i, symbol);
      Fa::StateNrType const state_nr_next = get_next_state(dsnc_, i, symbol);
      if (state_nr_next != state_nr_next_fa) {
        throw std::runtime_error("Transition mismatch for state " + std::to_string(i) + " and symbol " + std::to_string(symbol) + ": " + std::to_string(state_nr_next) + " != " + std::to_string(state_nr_next_fa));
      }
    }
  }

#endif
  return true;
}

auto FaToDsncTransitions::get_next_state(Dsnc const& dsnc_, uint64_t state_nr_, pmt::util::parse::Fa::SymbolType symbol_) -> uint64_t {
  uint64_t offset = dsnc_._transitions_shift[state_nr_] + symbol_;

  while (true) {
    if (offset < dsnc_._padding_l || offset >= dsnc_._transitions_next.size() + dsnc_._padding_l) {
      if (dsnc_._state_nr_most_frequent == state_nr_) {
        return dsnc_._state_nr_most_frequent;
      }
    } else {
      uint64_t const offset_adjusted = offset - dsnc_._padding_l;
      if (dsnc_._transitions_check[offset_adjusted] == state_nr_) {
        return dsnc_._transitions_next[offset_adjusted];
      }
    }

    // Update state_nr_ and offset for the next iteration
    state_nr_ = dsnc_._transitions_default[state_nr_];
    offset = dsnc_._transitions_shift[state_nr_] + symbol_;
  }
}

auto FaToDsncTransitions::get_next_state(pmt::util::parse::Fa const& fa_, uint64_t state_nr_, pmt::util::parse::Fa::SymbolType symbol_) -> uint64_t {
  Fa::Transitions const& transitions = fa_._states.find(state_nr_)->second._transitions;
  auto const itr = transitions._symbol_transitions.find(symbol_);
  return itr != transitions._symbol_transitions.end() ? itr->second : fa_._states.size();
}

}  // namespace pmt::parserbuilder