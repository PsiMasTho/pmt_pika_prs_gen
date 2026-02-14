#include "pmt/rt/pika_parser.hpp"

#include "pmt/rt/clause_base.hpp"
#include "pmt/rt/pika_tables_base.hpp"
#include "pmt/rt/reserved_ids.hpp"
#include "pmt/rt/rule_parameters_base.hpp"

#include <cassert>

namespace pmt::rt {

namespace {
constexpr auto is_ignored_tag(ClauseBase::Tag tag_) -> bool {
 return tag_ == ClauseBase::Tag::NegativeLookahead || tag_ == ClauseBase::Tag::Epsilon;
}

constexpr auto is_parent_tag(ClauseBase::Tag tag_) -> bool {
 switch (tag_) {
  case ClauseBase::Tag::Sequence:
  case ClauseBase::Tag::Choice:
  case ClauseBase::Tag::Identifier:
   return true;
  case ClauseBase::Tag::NegativeLookahead:
  case ClauseBase::Tag::Epsilon:
  case ClauseBase::Tag::CharsetLiteral:
   return false;
 }
 return false;
}

struct MemoEntry {
 MemoTable::IndexType _index;
 MemoTable::Key const* _key;
 MemoTable::Match const* _match;
};

auto memo_entry(MemoTable const& memo_table_, MemoTable::IndexType index_) -> MemoEntry {
 return MemoEntry{index_, &memo_table_.get_key_by_index(index_), &memo_table_.get_match_by_index(index_)};
}

auto make_string_node(std::string_view input_, MemoTable::Key const& key_, MemoTable::Match const& match_) -> pmt::rt::Ast::UniqueHandle {
 pmt::rt::Ast::UniqueHandle node = pmt::rt::Ast::construct(pmt::rt::Ast::Tag::String, pmt::rt::ReservedIds::IdDefault);
 node->set_string(Ast::StringType(input_.substr(key_._position, match_._length)));
 return node;
}

auto find_start_match_index_if_success(MemoTable const& memo_table_, std::string_view input_, PikaTablesBase const& pika_tables_) -> MemoTable::IndexType {
 ClauseBase const& start_clause = pika_tables_.fetch_clause(0);
 MemoTable::IndexType const start_match_index = memo_table_.find(MemoTable::Key{._clause = &start_clause, ._position = 0}, pika_tables_);
 if (start_match_index == MemoTable::MemoIndexMatchNotFound) {
  return MemoTable::MemoIndexMatchNotFound;
 }
 if (memo_table_.get_match_length_by_index(start_match_index) != input_.size()) {
  return MemoTable::MemoIndexMatchNotFound;
 }
 return start_match_index;
}

void process_and_add_child(pmt::rt::Ast& parent_, pmt::rt::Ast::UniqueHandle child_, MemoTable::Key const& child_key_, PikaTablesBase const& pika_tables_) {
 if (!child_) {
  return;
 }

 ClauseBase::Tag const child_tag = child_key_._clause->get_tag();
 switch (child_tag) {
  case ClauseBase::Tag::Sequence:
  case ClauseBase::Tag::Choice:
   parent_.give_child_at_back(std::move(child_));
   parent_.unpack(parent_.get_children_size() - 1);
   break;
  case ClauseBase::Tag::CharsetLiteral:
   parent_.give_child_at_back(std::move(child_));
   break;
  case ClauseBase::Tag::Identifier: {
   RuleParametersBase const& rule_parameters = pika_tables_.fetch_rule_parameters(child_key_._clause->get_rule_id());
   if (rule_parameters.get_hide()) {
    break;
   }
   if (rule_parameters.get_merge()) {
    child_->merge();
   }
   child_->set_id(rule_parameters.get_id_value());
   parent_.give_child_at_back(std::move(child_));
   if (rule_parameters.get_unpack()) {
    parent_.unpack(parent_.get_children_size() - 1);
   }
  } break;
  case ClauseBase::Tag::NegativeLookahead:
  case ClauseBase::Tag::Epsilon:
   break;
  default:
   assert(false && "UNREACHABLE");
 }
}

auto build_ast(MemoTable::IndexType const& root_match_index_, MemoTable const& memo_table_, std::string_view input_, PikaTablesBase const& pika_tables_) -> pmt::rt::Ast::UniqueHandle {
 MemoEntry const root = memo_entry(memo_table_, root_match_index_);
 ClauseBase::Tag const root_tag = root._key->_clause->get_tag();
 if (is_ignored_tag(root_tag)) {
  return nullptr;
 }
 if (root_tag == ClauseBase::Tag::CharsetLiteral) {
  return make_string_node(input_, *root._key, *root._match);
 }
 assert(is_parent_tag(root_tag));

 struct Frame {
  MemoTable::IndexType _index;
  MemoTable::Key const* _key;
  MemoTable::Match const* _match;
  size_t _next_child_idx;
  pmt::rt::Ast::UniqueHandle _ast_node;
 };

 std::vector<Frame> stack;
 stack.push_back(Frame{root._index, root._key, root._match, 0, pmt::rt::Ast::construct(pmt::rt::Ast::Tag::Parent, pmt::rt::ReservedIds::IdUninitialized)});

 pmt::rt::Ast::UniqueHandle result;

 while (!stack.empty()) {
  Frame& frame = stack.back();
  MemoTable::Match const& match = *frame._match;

  if (frame._next_child_idx >= match._matching_subclauses.size()) {
   pmt::rt::Ast::UniqueHandle completed = std::move(frame._ast_node);
   MemoTable::Key const* completed_key = frame._key;
   stack.pop_back();
   if (stack.empty()) {
    result = std::move(completed);
    break;
   }
   Frame& parent = stack.back();
   process_and_add_child(*parent._ast_node, std::move(completed), *completed_key, pika_tables_);
   continue;
  }

  MemoTable::IndexType const child_match_index = match._matching_subclauses[frame._next_child_idx++];
  if (child_match_index == MemoTable::MemoIndexMatchNotFound || child_match_index == MemoTable::MemoIndexMatchZeroLength) {
   continue;
  }

  MemoEntry const child = memo_entry(memo_table_, child_match_index);
  ClauseBase::Tag const child_tag = child._key->_clause->get_tag();
  if (is_parent_tag(child_tag)) {
   stack.push_back(Frame{child._index, child._key, child._match, 0, pmt::rt::Ast::construct(pmt::rt::Ast::Tag::Parent, pmt::rt::ReservedIds::IdUninitialized)});
  } else if (child_tag == ClauseBase::Tag::CharsetLiteral) {
   process_and_add_child(*frame._ast_node, make_string_node(input_, *child._key, *child._match), *child._key, pika_tables_);
  } else if (!is_ignored_tag(child_tag)) {
   assert(false && "UNREACHABLE");
  }
 }

 return result;
}

}  // namespace

auto PikaParser::memo_table_to_ast(MemoTable const& memo_table_, std::string_view input_, PikaTablesBase const& pika_tables_) -> pmt::rt::Ast::UniqueHandle {
 MemoTable::IndexType const start_match_index = find_start_match_index_if_success(memo_table_, input_, pika_tables_);
 if (start_match_index == MemoTable::MemoIndexMatchNotFound) {
  return nullptr;
 }

 pmt::rt::Ast::UniqueHandle ast_root = pmt::rt::Ast::construct(pmt::rt::Ast::Tag::Parent, pmt::rt::ReservedIds::IdRoot);

 if (start_match_index != MemoTable::MemoIndexMatchZeroLength) {
  MemoTable::Key const& start_key = memo_table_.get_key_by_index(start_match_index);
  process_and_add_child(*ast_root, build_ast(start_match_index, memo_table_, input_, pika_tables_), start_key, pika_tables_);
 }

 return ast_root;
}

}  // namespace pmt::rt
