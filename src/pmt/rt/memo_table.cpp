#include "pmt/rt/memo_table.hpp"

#include "pmt/hash.hpp"
#include "pmt/rt/clause_matcher.hpp"
#include "pmt/rt/pika_tables_base.hpp"

#include <cassert>

namespace pmt::rt {

MemoTable::KeyHashIndirect::KeyHashIndirect(std::vector<MemoTable::Key> const& keys_)
 : _keys(keys_) {
}

auto MemoTable::KeyHashIndirect::operator()(Key const& key_) const -> size_t {
 size_t seed = Hash::Phi64;
 Hash::combine(key_._clause, seed);
 Hash::combine(key_._position, seed);
 return seed;
}

auto MemoTable::KeyHashIndirect::operator()(IndexType index_) const -> size_t {
 assert(index_ < _keys.size());
 return operator()(_keys[index_]);
}

MemoTable::KeyEqIndirect::KeyEqIndirect(std::vector<MemoTable::Key> const& keys_)
 : _keys(keys_) {
}

auto MemoTable::KeyEqIndirect::fetch_key(Key const& key_) -> Key const& {
 return key_;
}

auto MemoTable::KeyEqIndirect::fetch_key(IndexType index_) const -> Key const& {
 assert(index_ < _keys.size());
 return _keys[index_];
}

MemoTable::MemoTable()
 : _table(0, KeyHashIndirect(_keys), KeyEqIndirect(_keys)) {
}

auto MemoTable::find(Key const& key_, std::string_view input_, PikaTablesBase const& pika_tables_) const -> IndexType {
 if (auto const itr = _table.find(key_); itr != _table.end()) {
  return itr->second;
 } else if (key_._clause->get_tag() == ClauseBase::Tag::NegativeLookahead) {
  return ClauseMatcher::match(*this, key_, input_, pika_tables_).has_value() ? MemoIndexMatchZeroLength : MemoIndexMatchNotFound;
 } else if (key_._clause->can_match_zero()) {
  return MemoIndexMatchZeroLength;
 }

 return MemoIndexMatchNotFound;
}

void MemoTable::insert(MemoTable::Key key_, std::optional<MemoTable::Match> new_match_, ClauseQueue& parse_queue_, PikaTablesBase const& pika_tables_) {
 bool match_updated = false;
 if (new_match_.has_value()) {
  auto const itr_old_match = _table.find(key_);
  if (itr_old_match == _table.end()) {
   _keys.push_back(key_);
   _matches.push_back(std::move(*new_match_));
   _matches.back()._key_index = _keys.size() - 1;
   _table.emplace(_keys.size() - 1, _matches.size() - 1);
   match_updated = true;
  } else if (new_match_->_length > get_match_length_by_index(itr_old_match->second)) {
   _matches.push_back(std::move(*new_match_));
   _matches.back()._key_index = itr_old_match->first;
   _table.insert_or_assign(_matches.back()._key_index, _matches.size() - 1);
   match_updated = true;
  }
 }

 for (size_t i = 0; i < key_._clause->get_seed_parent_count(); i++) {
  ClauseBase::IdType const seed_parent_id = key_._clause->get_seed_parent_id_at(i);
  ClauseBase const& seed_parent_clause = pika_tables_.fetch_clause(seed_parent_id);
  if (match_updated || seed_parent_clause.can_match_zero()) {
   ClauseQueueItem::PriorityType const priority = seed_parent_id;
   parse_queue_.push(ClauseQueueItem{._clause = &seed_parent_clause, ._priority = priority});
  }
 }
}

auto MemoTable::get_key_by_index(IndexType index_) const -> Key const& {
 assert(index_ < _keys.size());
 return _keys[index_];
}

auto MemoTable::get_match_by_index(IndexType index_) const -> Match const& {
 assert(index_ != MemoIndexMatchNotFound);
 assert(index_ != MemoIndexMatchZeroLength);
 assert(index_ < _matches.size());
 return _matches[index_];
}

auto MemoTable::get_match_length_by_index(IndexType index_) const -> size_t {
 return (index_ == MemoIndexMatchZeroLength) ? 0 : get_match_by_index(index_)._length;
}

auto MemoTable::get_match_count() const -> size_t {
 return _matches.size();
}

}  // namespace pmt::rt