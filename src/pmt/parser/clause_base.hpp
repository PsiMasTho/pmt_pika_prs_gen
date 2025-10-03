#pragma once

#include <cstddef>
#include <cstdint>

namespace pmt::parser {

class ClauseBase {
public:
 using IdType = uint64_t;

 enum class Tag : uint8_t {
  Sequence,       // >= 1 child id
  Choice,         // >= 1 child id
  Hidden,         // 1 child id
  Identifier,     // 1 child id
  LitSeq,         // 1 child id
  OneOrMore,      // 1 child id
  NotFollowedBy,  // 1 child id
  Epsilon,        // 0 child ids
 };

 virtual auto get_tag() const -> Tag = 0;
 virtual auto get_child_id_count() const -> size_t;
 virtual auto get_seed_parent_count() const -> size_t;
 virtual auto can_match_zero() const -> bool;
};

}  // namespace pmt::parser