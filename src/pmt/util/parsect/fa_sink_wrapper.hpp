#pragma once

#include "pmt/util/parsect/fa.hpp"

#include <optional>

namespace pmt::util::parsect {

class FaSinkWrapper {
 public:
  // - Public data -
  Fa const& _fa;

  // - Public functions -
  // -- Lifetime --
  FaSinkWrapper(Fa const& fa_);

  // -- Other --
  auto get_state_nr_next(Fa::StateNrType state_nr_, Fa::SymbolType symbol_) const -> Fa::StateNrType;
  auto get_state_nr_sink() const -> std::optional<Fa::StateNrType>;
  auto get_size() const -> size_t;

  void refresh();

 private:
  bool _has_sink;
};

}  // namespace pmt::util::parsect