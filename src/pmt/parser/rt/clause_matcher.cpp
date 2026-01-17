#include "pmt/parser/rt/clause_matcher.hpp"

#include "pmt/asserts.hpp"
#include "pmt/parser/rt/pika_program_base.hpp"

namespace pmt::parser::rt {
namespace {
auto match_sequence(MemoTable const& memo_table_, MemoTable::Key key_, std::string_view input_) -> std::optional<MemoTable::Match> {
 size_t length = 0;
 std::vector<MemoTable::IndexType> matching_subclauses;
 for (size_t i = 0; i < key_._clause->get_child_id_count(); ++i) {
  ClauseBase::IdType const child_id = key_._clause->get_child_id_at(i);
  MemoTable::Key child_key{._clause = &memo_table_.get_pika_program().fetch_clause(child_id), ._position = key_._position + length};
  MemoTable::IndexType const child_match_index = memo_table_.find(child_key);
  if (child_match_index == MemoTable::MemoIndexMatchNotFound) {
   return std::nullopt;
  }

  length += memo_table_.get_match_length_by_index(child_match_index);
  matching_subclauses.push_back(child_match_index);
 }

 return MemoTable::Match{._key = key_, ._length = length, ._matching_subclauses = matching_subclauses};
}

auto match_choice(MemoTable const& memo_table_, MemoTable::Key key_, std::string_view input_) -> std::optional<MemoTable::Match> {
 for (size_t i = 0; i < key_._clause->get_child_id_count(); ++i) {
  ClauseBase::IdType const child_id = key_._clause->get_child_id_at(i);
  MemoTable::Key child_key{._clause = &memo_table_.get_pika_program().fetch_clause(child_id), ._position = key_._position};
  MemoTable::IndexType const child_match_index = memo_table_.find(child_key);
  if (child_match_index != MemoTable::MemoIndexMatchNotFound) {
   return MemoTable::Match{._key = key_, ._length = memo_table_.get_match_length_by_index(child_match_index), ._matching_subclauses = {child_match_index}};
  }
 }
 return std::nullopt;
}

auto match_hidden(MemoTable const& memo_table_, MemoTable::Key key_, std::string_view input_) -> std::optional<MemoTable::Match> {
 ClauseBase::IdType const child_id = key_._clause->get_child_id_at(0);
 MemoTable::Key child_key{._clause = &memo_table_.get_pika_program().fetch_clause(child_id), ._position = key_._position};
 MemoTable::IndexType const child_match_index = memo_table_.find(child_key);
 if (child_match_index == MemoTable::MemoIndexMatchNotFound) {
  return std::nullopt;
 }
 return MemoTable::Match{._key = key_, ._length = memo_table_.get_match_length_by_index(child_match_index), ._matching_subclauses = {child_match_index}};
}

auto match_one_or_more(MemoTable const& memo_table_, MemoTable::Key key_, std::string_view input_) -> std::optional<MemoTable::Match> {
 ClauseBase::IdType const child_id = key_._clause->get_child_id_at(0);
 MemoTable::Key child_key{._clause = &memo_table_.get_pika_program().fetch_clause(child_id), ._position = key_._position};
 MemoTable::IndexType const child_match_index = memo_table_.find(child_key);
 if (child_match_index == MemoTable::MemoIndexMatchNotFound) {
  return std::nullopt;
 }

 size_t const child_match_length = memo_table_.get_match_length_by_index(child_match_index);
 MemoTable::Key tail_key{._clause = key_._clause, ._position = key_._position + child_match_length};
 MemoTable::IndexType const tail_match_index = memo_table_.find(tail_key);
 if (tail_match_index == MemoTable::MemoIndexMatchNotFound) {
  return MemoTable::Match{._key = key_, ._length = child_match_length, ._matching_subclauses = {child_match_index}};
 }
 size_t const tail_match_length = memo_table_.get_match_length_by_index(tail_match_index);
 return MemoTable::Match{._key = key_, ._length = child_match_length + tail_match_length, ._matching_subclauses = {child_match_index, tail_match_index}};
}

auto match_not_followed_by(MemoTable const& memo_table_, MemoTable::Key key_, std::string_view input_) -> std::optional<MemoTable::Match> {
 ClauseBase::IdType const child_id = key_._clause->get_child_id_at(0);
 MemoTable::Key child_key{._clause = &memo_table_.get_pika_program().fetch_clause(child_id), ._position = key_._position};
 MemoTable::IndexType const child_match_index = memo_table_.find(child_key);
 if (child_match_index != MemoTable::MemoIndexMatchNotFound) {
  return std::nullopt;
 }
 return MemoTable::Match{._key = key_, ._length = 0};
}

auto match_epsilon(MemoTable const& memo_table_, MemoTable::Key key_, std::string_view input_) -> std::optional<MemoTable::Match> {
 return MemoTable::Match{._key = key_, ._length = 0};
}

auto match_identifier(MemoTable const& memo_table_, MemoTable::Key key_, std::string_view input_) -> std::optional<MemoTable::Match> {
 ClauseBase::IdType const child_id = key_._clause->get_child_id_at(0);
 MemoTable::Key child_key{._clause = &memo_table_.get_pika_program().fetch_clause(child_id), ._position = key_._position};
 MemoTable::IndexType const child_match_index = memo_table_.find(child_key);
 if (child_match_index == MemoTable::MemoIndexMatchNotFound) {
  return std::nullopt;
 }
 return MemoTable::Match{._key = key_, ._length = memo_table_.get_match_length_by_index(child_match_index), ._matching_subclauses = {child_match_index}};
}

}  // namespace

auto ClauseMatcher::match(MemoTable const& memo_table_, MemoTable::Key key_, std::string_view input_) -> std::optional<MemoTable::Match> {
 switch (key_._clause->get_tag()) {
  case ClauseBase::Tag::Sequence:
   return match_sequence(memo_table_, key_, input_);
  case ClauseBase::Tag::Choice:
   return match_choice(memo_table_, key_, input_);
  case ClauseBase::Tag::Hidden:
   return match_hidden(memo_table_, key_, input_);
  case ClauseBase::Tag::OneOrMore:
   return match_one_or_more(memo_table_, key_, input_);
  case ClauseBase::Tag::NegativeLookahead:
   return match_not_followed_by(memo_table_, key_, input_);
  case ClauseBase::Tag::Epsilon:
   return match_epsilon(memo_table_, key_, input_);
  case ClauseBase::Tag::Identifier:
   return match_identifier(memo_table_, key_, input_);
  case ClauseBase::Tag::CharsetLiteral:
  default:
   pmt::unreachable();
 }
}
}  // namespace pmt::parser::rt