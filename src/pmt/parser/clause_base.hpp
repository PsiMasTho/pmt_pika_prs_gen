#pragma once

#include <cstddef>
#include <cstdint>
#include <string>

namespace pmt::parser {

class ClauseBase {
public:
 using IdType = uint64_t;

 enum class Tag : uint8_t {
  Sequence,       // >= 1 child id
  Choice,         // >= 1 child id
  Hidden,         // 1 child id
  Identifier,     // NOT IN USE
  Literal,        // 1 child id
  OneOrMore,      // 1 child id
  NotFollowedBy,  // 1 child id
  Epsilon,        // 0 child ids
 };

 [[nodiscard]] virtual auto get_tag() const -> Tag = 0;

 [[nodiscard]] virtual auto get_child_id_at(size_t idx_) const -> ClauseBase::IdType = 0;
 [[nodiscard]] virtual auto get_child_id_count() const -> size_t = 0;

 [[nodiscard]] virtual auto get_registered_rule_id_at(size_t idx_) const -> ClauseBase::IdType = 0;
 [[nodiscard]] virtual auto get_registered_rule_id_count() const -> size_t = 0;

 [[nodiscard]] virtual auto get_seed_parent_id_at(size_t idx_) const -> ClauseBase::IdType = 0;
 [[nodiscard]] virtual auto get_seed_parent_count() const -> size_t = 0;

 [[nodiscard]] virtual auto can_match_zero() const -> bool = 0;

 [[nodiscard]] static auto tag_to_string(Tag tag_) -> std::string;
};

}  // namespace pmt::parser