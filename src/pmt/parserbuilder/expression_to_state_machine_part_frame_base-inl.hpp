// clang-format off
#ifdef __INTELLISENSE__
 #include "pmt/parserbuilder/expression_to_state_machine_part_frame_base.hpp"
#endif
// clang-format on

namespace pmt::parserbuilder {

template <util::parsect::IsStateTag TAG_>
ExpressionToStateMachinePartFrameBase<TAG_>::ExpressionToStateMachinePartFrameBase(pmt::util::parsert::GenericAstPath const& path_)
 : _path(path_) {
}

}  // namespace pmt::parserbuilder