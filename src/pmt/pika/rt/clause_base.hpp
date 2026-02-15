#pragma once

#include "pmt/pika/rt/primitives.hpp"

#include <cstddef>
#include <cstdint>
#include <string>

namespace pmt::pika::rt {

class ClauseBase {
public:
 // -$ Types / Constants $-
 enum class Tag : uint8_t {
  Sequence,           // >= 1 child ids
  Choice,             // >= 1 child ids
  Identifier,         // 1 child id + 1 rule id
  CharsetLiteral,     // 1 literal id
  NegativeLookahead,  // 1 child id
  Epsilon,
 };

 // -$ Functions $-
 // --$ Virtual $--
 [[nodiscard]] virtual auto get_tag() const -> Tag = 0;
 [[nodiscard]] virtual auto get_id() const -> IdType = 0;

 [[nodiscard]] virtual auto get_child_id_at(size_t idx_) const -> IdType = 0;
 [[nodiscard]] virtual auto get_child_id_count() const -> size_t = 0;

 [[nodiscard]] virtual auto get_literal_id() const -> IdType = 0;
 [[nodiscard]] auto has_literal_id() const -> bool;

 [[nodiscard]] virtual auto get_rule_id() const -> IdType = 0;
 [[nodiscard]] auto has_rule_id() const -> bool;

 [[nodiscard]] virtual auto get_seed_parent_id_at(size_t idx_) const -> IdType = 0;
 [[nodiscard]] virtual auto get_seed_parent_count() const -> size_t = 0;

 [[nodiscard]] virtual auto can_match_zero() const -> bool = 0;

 // --$ Other $--
 [[nodiscard]] static auto tag_to_string(Tag tag_) -> std::string;
};

}  // namespace pmt::pika::rt