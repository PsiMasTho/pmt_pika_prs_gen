#pragma once

#include "pmt/util/parsect/fa.hpp"

namespace pmt::parserbuilder {

class FaSinkWrapper {
 public:
  // - Public data -
  pmt::util::parsect::Fa const& _fa;

  // - Public functions -
  // -- Lifetime --
  FaSinkWrapper(pmt::util::parsect::Fa const& fa_);

  // -- Other --
  auto get_state_nr_next(pmt::util::parsect::Fa::StateNrType state_nr_, pmt::util::parsect::Fa::SymbolType symbol_) const -> pmt::util::parsect::Fa::StateNrType;
  auto get_state_nr_sink() const -> pmt::util::parsect::Fa::StateNrType;
  auto get_size() const -> size_t;
};

}  // namespace pmt::parserbuilder