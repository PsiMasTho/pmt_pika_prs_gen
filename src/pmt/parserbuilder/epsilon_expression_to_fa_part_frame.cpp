#include "pmt/parserbuilder/epsilon_expression_to_fa_part_frame.hpp"

namespace pmt::parserbuilder {
using namespace pmt::util::parsect;
using namespace pmt::util::parsert;

void EpsilonExpressionToFaPartFrame::process(CallstackType&, Captures& captures_) {
  captures_._ret_part = make_epsilon(captures_._result);
}

auto EpsilonExpressionToFaPartFrame::make_epsilon(Fa& fa_) -> FaPart {
  Fa::StateNrType state_nr_incoming = fa_.get_unused_state_nr();
  fa_._states[state_nr_incoming];
  FaPart ret(state_nr_incoming);
  ret.add_outgoing_epsilon_transition(state_nr_incoming);
  return ret;
}

}  // namespace pmt::parserbuilder