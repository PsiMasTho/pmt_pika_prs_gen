#include "pmt/parserbuilder/expression_to_fa_part_frame_base.hpp"

namespace pmt::parserbuilder {

using namespace pmt::util::parsert;

ExpressionToFaPartFrameBase::ExpressionToFaPartFrameBase(GenericAstPath const& path_)
 : _path(path_) {
}

}  // namespace pmt::parserbuilder