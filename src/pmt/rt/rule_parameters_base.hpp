#pragma once

#include "pmt/rt/ast_id.hpp"

#include <string_view>

namespace pmt::rt {

class RuleParametersBase {
 // -$ Types / Constants $-
public:
 static inline bool const MERGE_DEFAULT = false;
 static inline bool const UNPACK_DEFAULT = false;
 static inline bool const HIDE_DEFAULT = false;

 // --$ Other $--
 [[nodiscard]] virtual auto get_display_name() const -> std::string_view = 0;
 [[nodiscard]] virtual auto get_id_string() const -> std::string_view = 0;
 [[nodiscard]] virtual auto get_id_value() const -> AstId::IdType = 0;
 [[nodiscard]] virtual auto get_merge() const -> bool = 0;
 [[nodiscard]] virtual auto get_unpack() const -> bool = 0;
 [[nodiscard]] virtual auto get_hide() const -> bool = 0;
};

}  // namespace pmt::rt