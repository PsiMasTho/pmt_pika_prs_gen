#pragma once

#include "pmt/rt/clause_queue.hpp"

#include <limits>
#include <optional>
#include <unordered_map>
#include <vector>

namespace pmt::rt {
class PikaTablesBase;
class ClauseBase;

class MemoTable {
public:
 // -$ Types / Constants $-
 using IndexType = uint32_t;

 enum : IndexType {
  MemoIndexKeyUninitialized = std::numeric_limits<IndexType>::max(),
  MemoIndexMatchNotFound = std::numeric_limits<IndexType>::max(),
  MemoIndexMatchZeroLength = MemoIndexMatchNotFound - 1,
 };

 class Key {
 public:
  ClauseBase const* _clause;
  size_t _position;
 };

 class KeyHashIndirect {
  std::vector<Key> const& _keys;

 public:
  using is_transparent = void;  // NOLINT
  KeyHashIndirect(std::vector<Key> const& keys_);
  auto operator()(Key const& key_) const -> size_t;
  auto operator()(IndexType index_) const -> size_t;
 };

 class KeyEqIndirect {
  std::vector<Key> const& _keys;

 public:
  using is_transparent = void;  // NOLINT
  KeyEqIndirect(std::vector<Key> const& keys_);
  auto operator()(Key const& lhs_, Key const& rhs_) const -> bool;
  auto operator()(auto const& lhs_, auto const& rhs_) const -> bool;

 private:
  static auto fetch_key(Key const& key_) -> Key const&;
  auto fetch_key(IndexType index_) const -> Key const&;
 };

 class Match {
 public:
  size_t _length;
  std::vector<IndexType> _matching_subclauses;
 };

private:
 // -$ Data $-
 std::unordered_map<IndexType, IndexType, KeyHashIndirect, KeyEqIndirect> _table;  // <KeyIndex, MatchIndex>
 std::vector<Key> _keys;
 std::vector<Match> _matches;

public:
 // -$ Functions $-
 // --$ Lifetime $--
 MemoTable();
 // --$ Other $--
 auto find(Key const& key_, PikaTablesBase const& pika_tables_) const -> IndexType;  // -> MatchIndex (which may be MemoIndexMatchNotFound / MemoIndexMatchZeroLength)
 void insert(Key key_, std::optional<Match> new_match_, ClauseQueue& parse_queue_, PikaTablesBase const& pika_tables_);

 auto get_key_by_index(IndexType index_) const -> Key const&;
 auto get_match_by_index(IndexType index_) const -> Match const&;
 auto get_match_length_by_index(IndexType index_) const -> size_t;

 auto get_match_count() const -> size_t;

private:
 auto match_negative_lookahead_topdown(Key const& key_, PikaTablesBase const& pika_tables_) const -> IndexType;
};

}  // namespace pmt::rt

#include "pmt/rt/memo_table-inl.hpp"
