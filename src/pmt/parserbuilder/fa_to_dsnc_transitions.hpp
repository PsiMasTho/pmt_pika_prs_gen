#pragma once

#include "pmt/util/parse/fa.hpp"

#include <cstdint>
#include <unordered_set>
#include <vector>

namespace pmt::parserbuilder {

// Default-Shift-Next-Check Transition table
class Dsnc {
 public:
  pmt::util::parse::Fa::StateNrType _state_nr_invalid;
  std::vector<uint64_t> _transitions_default;
  std::vector<uint64_t> _transitions_shift;
  std::vector<uint64_t> _transitions_next;
  std::vector<uint64_t> _transitions_check;
};

class FaToDsncTransitions {
 public:
  FaToDsncTransitions(pmt::util::parse::Fa const& fa_);

  auto convert() -> Dsnc;

 private:
  void step_0(Dsnc& dsnc_);
  void step_1(Dsnc& dsnc_);
  void step_2(Dsnc& dsnc_);

  auto debug_pre_checks() const -> bool;
  auto debug_post_checks(Dsnc const& dsnc_) const -> bool;

  static auto get_next_state(Dsnc const& dsnc_, uint64_t state_nr_, pmt::util::parse::Fa::SymbolType symbol_) -> uint64_t;
  static auto get_next_state(pmt::util::parse::Fa const& fa_, uint64_t state_nr_, pmt::util::parse::Fa::SymbolType symbol_) -> uint64_t;

  std::vector<pmt::util::parse::Fa::StateNrType> _ordering;
  std::vector<std::unordered_set<pmt::util::parse::Fa::SymbolType>> _keep;
  pmt::util::parse::Fa const& _fa;
};

}  // namespace pmt::parserbuilder