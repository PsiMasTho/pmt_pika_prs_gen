#pragma once

#include "pmt/fw_decl.hpp"

#include <cstddef>
#include <utility>

PMT_FW_DECL_NS_CLASS(pmt::util::parse, GenericAst);

namespace pmt::util::parse {
using AstPosition = std::pair<GenericAst *, size_t>;
using AstPositionConst = std::pair<GenericAst const *, size_t>;
}  // namespace pmt::util::parse