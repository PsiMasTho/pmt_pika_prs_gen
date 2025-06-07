#pragma once

#include "pmt/fw_decl.hpp"

#include <functional>
#include <string_view>

PMT_FW_DECL_NS_CLASS(pmt::util::smct, StateMachine)

namespace pmt::parserbuilder {

class ParserReachabilityChecker {
 public:
  // -$ Types / Constants $-
  using AcceptLookupFn = std::function<size_t(std::string_view)>;

  class Args {
   public:
   AcceptLookupFn _fn_lookup_accept_index_by_label;
   pmt::util::smct::StateMachine const& _parser_state_machine;
  };

  // -$ Functions $-
  // --$ Other $--
  static void check_reachability(Args args_);
};
}