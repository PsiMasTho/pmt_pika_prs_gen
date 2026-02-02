#pragma once

#include <cstddef>
#include <cstdint>
#include <limits>
#include <string>

namespace pmt::rt {

class ClauseBase {
public:
 // -$ Types / Constants $-
 using IdType = uint32_t;

 enum class Tag : uint8_t {
  // Child IDs
  Sequence,           // >= 1 clause ids
  Choice,             // >= 1 clause ids
  Identifier,         // 1 clause id + 1 rule id
  CharsetLiteral,     // 1 literal id
  OneOrMore,          // 1 clause id
  NegativeLookahead,  // 1 clause id
  Eof,                // not stored in memo table
  Epsilon,            // not stored
 };

 enum : IdType {
  IdInvalid = std::numeric_limits<IdType>::max(),
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

}  // namespace pmt::rt