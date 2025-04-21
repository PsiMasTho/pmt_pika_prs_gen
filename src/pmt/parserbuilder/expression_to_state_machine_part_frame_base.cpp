#include "pmt/parserbuilder/expression_to_state_machine_part_frame_base.hpp"

namespace pmt::parserbuilder {

ExpressionToStateMachinePartFrameBase::ExpressionToStateMachinePartFrameBase(pmt::util::smrt::GenericAstPath const& path_)
 : _path(path_) {
}

}  // namespace pmt::parserbuilder