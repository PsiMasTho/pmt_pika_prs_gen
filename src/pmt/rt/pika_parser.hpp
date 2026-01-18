#pragma once

#include "pmt/rt/ast.hpp"

#include <string_view>

namespace pmt::rt {
class PikaProgramBase;

class PikaParser {
public:
 // -$ Types / Constants $-
 // -$ Functions $-
 // --$ Other $--
 static auto parse(PikaProgramBase const& pika_program_, std::string_view input_) -> Ast::UniqueHandle;
};

}  // namespace pmt::rt