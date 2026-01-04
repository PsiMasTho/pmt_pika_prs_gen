#pragma once

#include "pmt/base/hashable.hpp"
#include "pmt/fw_decl.hpp"
#include "pmt/parser/clause_base.hpp"
#include "pmt/parser/rt/clause_queue.hpp"

#include <limits>
#include <optional>
#include <string_view>
#include <unordered_map>
#include <vector>

PMT_FW_DECL_NS_CLASS(pmt::parser::rt, PikaProgramBase);

namespace pmt::parser::rt {

class MemoTable {
public:
 // -$ Types / Constants $-
 using IndexType = size_t;

 enum : IndexType {
  MemoIndexMatchNotFound = std::numeric_limits<IndexType>::max(),
  MemoIndexMatchZeroLength = MemoIndexMatchNotFound - 1,
 };

 class Key : public pmt::base::Hashable<Key> {
 public:
  ClauseBase const* _clause;
  size_t _position;

  auto hash() const -> size_t;
 };

 class Match {
 public:
  Key _key;
  size_t _length;
  std::vector<IndexType> _matching_subclauses;
 };

private:
 // -$ Data $-
 std::unordered_map<Key, IndexType> _table;
 std::vector<Match> _matches;
 std::string_view _input;
 PikaProgramBase const& _pika_program;

public:
 // -$ Functions $-
 // --$ Lifetime $--
 MemoTable(PikaProgramBase const& pika_program_, std::string_view input_);

 // --$ Other $--
 auto find(Key const& key_) const -> IndexType;
 void insert(Key key_, std::optional<Match> new_match_, ClauseQueue& parse_queue_);

 auto get_match_by_index(IndexType index_) const -> Match const&;
 auto get_match_length_by_index(IndexType index_) const -> size_t;
 auto get_match_count() const -> size_t;

 auto get_pika_program() const -> PikaProgramBase const&;
 auto get_input() const -> std::string_view;
};

}  // namespace pmt::parser::rt