#include "pmt/parserbuilder/expression_to_fa_part_frame_base.hpp"

namespace pmt::parserbuilder {

using namespace pmt::util::parsert;

ExpressionToFaPartFrameBase::ExpressionToFaPartFrameBase(GenericAst::AstPositionConst ast_position_)
 : _ast_position(std::move(ast_position_)) {
}

}  // namespace pmt::parserbuilder