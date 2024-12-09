#include "pmt/parserbuilder/fa_to_dsnc_transitions.hpp"

#include "pmt/base/dynamic_bitset.hpp"
#include "pmt/util/parse/fa.hpp"

#include <algorithm>
#include <chrono>
#include <cstdint>
#include <iostream>
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

  std::chrono::steady_clock::time_point const start = std::chrono::steady_clock::now();
  Dsnc dsnc;
  step_0(dsnc);
  step_1(dsnc);
  step_2(dsnc);
  std::chrono::steady_clock::time_point const end = std::chrono::steady_clock::now();
  std::cout << "Conversion took: " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << "ms\n";

  debug_post_checks(dsnc);

  return dsnc;
}

void FaToDsncTransitions::step_0(Dsnc& dsnc_) {
  dsnc_._state_nr_invalid = _fa._states.size();
  dsnc_._transitions_default.resize(_fa._states.size() + 1, std::numeric_limits<uint64_t>::max());

  std::generate_n(std::back_inserter(_ordering), _fa._states.size() + 1, [i = 0]() mutable { return i++; });

  std::vector<size_t> frequencies(_fa._states.size() + 1, 0);
  for (auto const& [state_nr, state] : _fa._states) {
    DynamicBitset alphabet(UCHAR_MAX, 0);
    for (auto const& [symbol, state_nr_next] : state._transitions._symbol_transitions) {
      ++frequencies[state_nr_next];
      alphabet.set(static_cast<unsigned char>(symbol), true);
    }
    frequencies[dsnc_._state_nr_invalid] += alphabet.size() - alphabet.popcnt();
  }

  std::ranges::sort(_ordering, [&frequencies](size_t lhs_, size_t rhs_) { return frequencies[lhs_] > frequencies[rhs_]; });
}

void FaToDsncTransitions::step_1(Dsnc& dsnc_) {
  _keep.resize(_fa._states.size() + 1);

  std::unordered_set<Fa::SymbolType> keep_all;
  for (Fa::SymbolType i = 0; i < UCHAR_MAX; ++i) {
    keep_all.insert(i);
  }

  dsnc_._transitions_default[_ordering.front()] = _ordering.front();
  _keep[_ordering.front()] = {};

  // For every state, pick the most similar state that appears before it in the ordering,
  // and keep track of the symbols that differ between the two states
  for (size_t i = 1; i < _ordering.size(); ++i) {
    Fa::StateNrType const state_nr_i = _ordering[i];

    Fa::StateNrType state_nr_best = _ordering.front();
    std::unordered_set<Fa::SymbolType> keep_best = keep_all;

    for (Fa::StateNrType j = 0; j < i; ++j) {
      Fa::StateNrType const state_nr_j = _ordering[j];

      std::unordered_set<Fa::SymbolType> keep_cur;
      for (Fa::SymbolType k = 0; k < UCHAR_MAX; ++k) {
        if (get_next_state(_fa, state_nr_i, k) != get_next_state(_fa, state_nr_j, k)) {
          keep_cur.insert(k);
        }
      }

      size_t const popcnt_cur = keep_cur.size();
      if (popcnt_cur < keep_best.size()) {
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

void FaToDsncTransitions::step_2(Dsnc& dsnc_) {
  std::unordered_map<Fa::StateNrType, uint64_t> shifts;
  std::unordered_map<size_t, std::pair<Fa::StateNrType, Fa::StateNrType>> occupied;  // <index, which state filled it, state_nr_to>
  size_t shift_max = 0;
  for (Fa::StateNrType state_nr_i : _ordering) {
    std::unordered_map<size_t, Fa::StateNrType> kept;
    for (Fa::SymbolType j = 0; j < UCHAR_MAX; ++j) {
      if (!_keep[state_nr_i].contains(j)) {
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

  dsnc_._transitions_shift.resize(_fa._states.size() + 1, std::numeric_limits<uint64_t>::max());
  dsnc_._transitions_next.resize(shift_max + UCHAR_MAX, dsnc_._state_nr_invalid);
  dsnc_._transitions_check.resize(shift_max + UCHAR_MAX, dsnc_._state_nr_invalid);

  for (auto const& [i, shift] : shifts) {
    dsnc_._transitions_shift[i] = shift;
  }

  for (auto const& [i, j] : occupied) {
    auto const& [check, next] = j;
    dsnc_._transitions_next[i] = next;
    dsnc_._transitions_check[i] = check;
  }

  // pop any trailing invalid from next. During lookup if we index out of bounds, we assume that was the invalid state
  while (!dsnc_._transitions_next.empty() && dsnc_._transitions_next.back() == dsnc_._state_nr_invalid) {
    dsnc_._transitions_next.pop_back();
    dsnc_._transitions_check.pop_back();
  }
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
  // print size info
  std::cout << "-- Post checks --\n";
  std::cout << "FA State count: " << _fa._states.size() << '\n';
  std::cout << "DSMC::invalid: " << dsnc_._state_nr_invalid << '\n';
  std::cout << "DSNC::default size: " << dsnc_._transitions_default.size() << '\n';
  std::cout << "DSNC::shift size: " << dsnc_._transitions_shift.size() << '\n';
  std::cout << "DSNC::next size: " << dsnc_._transitions_next.size() << '\n';
  std::cout << "DSNC::check size: " << dsnc_._transitions_check.size() << '\n';
  std::cout << '\n';
  std::cout << "Order: ";
  std::string delim;
  for (auto const& i : _ordering) {
    std::cout << std::exchange(delim, ", ") << i;
  }
  std::cout << "\n\n";
  std::cout << "Next: ";
  delim.clear();
  for (auto const& i : dsnc_._transitions_next) {
    std::cout << std::exchange(delim, ", ") << i;
  }
  std::cout << "\n\n";
  std::cout << "Default: ";
  delim.clear();
  for (auto const& i : dsnc_._transitions_default) {
    std::cout << std::exchange(delim, ", ") << i;
  }
  std::cout << "\n\n";
  std::cout << "Check: ";
  delim.clear();
  for (auto const& i : dsnc_._transitions_check) {
    std::cout << std::exchange(delim, ", ") << i;
  }
  std::cout << "\n\n";
  std::cout << "Shift: ";
  delim.clear();
  for (auto const& i : dsnc_._transitions_shift) {
    std::cout << std::exchange(delim, ", ") << i;
  }
  std::cout << "\n\n";

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
    if (dsnc_._transitions_check.size() < offset || state_nr_ == dsnc_._state_nr_invalid) {
      return dsnc_._state_nr_invalid;
    }

    if (dsnc_._transitions_check[offset] == state_nr_) {
      return dsnc_._transitions_next[offset];
    }

    // Update state_nr_ and offset for the next iteration
    state_nr_ = dsnc_._transitions_default[state_nr_];
    offset = dsnc_._transitions_shift[state_nr_] + symbol_;
  }
}

auto FaToDsncTransitions::get_next_state(pmt::util::parse::Fa const& fa_, uint64_t state_nr_, pmt::util::parse::Fa::SymbolType symbol_) -> uint64_t {
  if (state_nr_ == fa_._states.size()) {
    return fa_._states.size();
  }
  Fa::Transitions const& transitions = fa_._states.find(state_nr_)->second._transitions;
  auto const itr = transitions._symbol_transitions.find(symbol_);
  return itr != transitions._symbol_transitions.end() ? itr->second : fa_._states.size();
}

}  // namespace pmt::parserbuilder