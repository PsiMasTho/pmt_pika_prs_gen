#include "pmt/parser/grammar/util.hpp"

#include "pmt/base/interval_container_common.hpp"
#include "pmt/parser//util.hpp"
#include "pmt/parser/primitives.hpp"

#include <cassert>

namespace pmt::parser::grammar {
using namespace pmt::base;

namespace {

auto unprintable_symbol_to_string(SymbolType symbol_) {
 assert(!is_symbol_printable(symbol_));
 return "10#" + std::to_string(symbol_);
}

auto symbol_to_string(SymbolType symbol_) -> std::string {
 return is_symbol_printable(symbol_) ? std::string(1, symbol_) : unprintable_symbol_to_string(symbol_);
}

}  // namespace

auto literal_sequence_to_printable_string(std::vector<pmt::base::IntervalSet<SymbolType>> const& literal_) -> std::string {
 std::string ret;

 size_t i = 0;
 size_t const n = literal_.size();

 while (i < n) {
  bool in_printable = false;
  while (i < n && literal_[i].get_by_index(0).get_lower() == literal_[i].get_by_index(0).get_upper() && is_symbol_printable(literal_[i].get_by_index(0).get_lower())) {
   ret += in_printable ? "" : "\"";
   ret += +literal_[i].get_by_index(0).get_lower();
   ++i;
   in_printable = true;
  }
  ret += in_printable ? "\"" : "";

  if (i >= n) {
   break;
  }

  while (i < n && literal_[i].get_by_index(0).get_lower() == literal_[i].get_by_index(0).get_upper() && !is_symbol_printable(literal_[i].get_by_index(0).get_lower())) {
   ret += unprintable_symbol_to_string(literal_[i].get_by_index(0).get_lower());
   ++i;
  }

  if (i >= n) {
   break;
  }

  while (i < n && literal_[i].get_by_index(0).get_lower() != literal_[i].get_by_index(0).get_upper()) {
   ret += "[";
   literal_[i].for_each_interval([&](Interval<SymbolType> interval_) { ret += symbol_to_string(interval_.get_lower()) + ".." + symbol_to_string(interval_.get_upper()); });
   ret += "]";
   ++i;
  }
 }

 return ret;
}

}  // namespace pmt::parser::grammar