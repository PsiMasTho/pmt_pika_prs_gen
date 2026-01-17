#include "pmt/parser/rt/memo_table.hpp"

#include "pmt/base/hash.hpp"
#include "pmt/parser/rt/clause_matcher.hpp"
#include "pmt/parser/rt/pika_program_base.hpp"

#include <cassert>

namespace pmt::parser::rt {
using namespace pmt::base;

auto MemoTable::Key::hash() const -> size_t {
 size_t seed = Hash::Phi64;
 Hash::combine(_clause, seed);
 Hash::combine(_position, seed);
 return seed;
}

MemoTable::MemoTable(PikaProgramBase const& pika_program_, std::string_view input_)
 : _input(input_)
 , _pika_program(pika_program_) {
}

auto MemoTable::find(Key const& key_) const -> IndexType {
 if (auto const itr = _table.find(key_); itr != _table.end()) {
  return itr->second;
 } else if (key_._clause->get_tag() == ClauseBase::Tag::NegativeLookahead) {
  return ClauseMatcher::match(*this, key_, _input).has_value() ? MemoIndexMatchZeroLength : MemoIndexMatchNotFound;
 } else if (key_._clause->can_match_zero()) {
  return MemoIndexMatchZeroLength;
 }

 return MemoIndexMatchNotFound;
}

void MemoTable::insert(MemoTable::Key key_, std::optional<MemoTable::Match> new_match_, ClauseQueue& parse_queue_) {
 bool match_updated = false;
 if (new_match_.has_value()) {
  auto const itr_old_match = _table.find(key_);

  if ((itr_old_match == _table.end() || new_match_->_length > get_match_by_index(itr_old_match->second)._length)) {
   _matches.push_back(std::move(new_match_.value()));
   _table.insert_or_assign(key_, _matches.size() - 1);
   match_updated = true;
  }
 }
 for (int i = 0, ii = key_._clause->get_seed_parent_count(); i < ii; i++) {
  ClauseBase::IdType const seed_parent_id = key_._clause->get_seed_parent_id_at(i);
  ClauseBase const& seed_parent_clause = _pika_program.fetch_clause(seed_parent_id);
  if (match_updated || seed_parent_clause.can_match_zero()) {
   int const priority = seed_parent_id;
   parse_queue_.push(ClauseQueueItem{._clause = &seed_parent_clause, ._priority = priority});
  }
 }
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

auto MemoTable::get_pika_program() const -> PikaProgramBase const& {
 return _pika_program;
}

auto MemoTable::get_input() const -> std::string_view {
 return _input;
}

}  // namespace pmt::parser::rt