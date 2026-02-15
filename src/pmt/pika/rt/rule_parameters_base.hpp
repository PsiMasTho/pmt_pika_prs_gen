#pragma once

#include "pmt/pika/rt/reserved_ids.hpp"

#include <string_view>

namespace pmt::pika::rt {

class RuleParametersBase {
 // -$ Types / Constants $-
public:
 static inline IdType const ID_VALUE_DEFAULT = ReservedIds::IdDefault;
 static inline bool const MERGE_DEFAULT = false;
 static inline bool const UNPACK_DEFAULT = false;
 static inline bool const HIDE_DEFAULT = false;

 // -$ Functions $-
 // --$ Virtual $--
 [[nodiscard]] virtual auto get_display_name() const -> std::string_view = 0;
 [[nodiscard]] virtual auto get_id_value() const -> IdType = 0;
 [[nodiscard]] virtual auto get_merge() const -> bool = 0;
 [[nodiscard]] virtual auto get_unpack() const -> bool = 0;
 [[nodiscard]] virtual auto get_hide() const -> bool = 0;
};

}  // namespace pmt::pika::rt