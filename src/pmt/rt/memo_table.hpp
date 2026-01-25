#pragma once

#include "pmt/rt/clause_base.hpp"
#include "pmt/rt/clause_queue.hpp"

#include <limits>
#include <optional>
#include <string_view>
#include <unordered_map>
#include <vector>

namespace pmt::rt {
class PikaProgramBase;

class MemoTable {
public:
 // -$ Types / Constants $-
 using IndexType = size_t;

 enum : IndexType {
  MemoIndexMatchNotFound = std::numeric_limits<IndexType>::max(),
  MemoIndexMatchZeroLength = MemoIndexMatchNotFound - 1,
 };

 class Key {
 public:
  ClauseBase const* _clause;
  size_t _position;
 };

 class KeyHash {
 public:
  auto operator()(Key const& key_) const -> size_t;
 };

 class KeyEq {
 public:
  auto operator()(Key const& lhs_, Key const& rhs_) const -> bool;
 };

 class Match {
 public:
  Key _key;
  size_t _length;
  std::vector<IndexType> _matching_subclauses;
 };

private:
 // -$ Data $-
 std::unordered_map<Key, IndexType, KeyHash, KeyEq> _table;
 std::vector<Match> _matches;

public:
 // -$ Functions $-
 // --$ Lifetime $--
 // --$ Other $--
 auto find(Key const& key_, std::string_view input_, PikaProgramBase const& pika_program_) const -> IndexType;
 void insert(Key key_, std::optional<Match> new_match_, ClauseQueue& parse_queue_, PikaProgramBase const& pika_program_);

 auto get_match_by_index(IndexType index_) const -> Match const&;
 auto get_match_length_by_index(IndexType index_) const -> size_t;
 auto get_match_count() const -> size_t;
};

}  // namespace pmt::rt
