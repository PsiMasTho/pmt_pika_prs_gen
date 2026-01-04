#pragma once

#include "pmt/fw_decl.hpp"
#include "pmt/parser/generic_ast.hpp"

#include <string_view>

PMT_FW_DECL_NS_CLASS(pmt::parser::rt, PikaProgramBase);

namespace pmt::parser::rt {

class PikaParser {
public:
 // -$ Types / Constants $-
 // -$ Functions $-
 // --$ Other $--
 static auto parse(PikaProgramBase const& pika_program_, std::string_view input_) -> GenericAst::UniqueHandle;
};

}  // namespace pmt::parser::rt