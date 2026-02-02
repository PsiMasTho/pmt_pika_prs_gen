#include "pmt/rt/pika_parser.hpp"

#include "pmt/rt/pika_tables_base.hpp"
#include "pmt/rt/rule_parameters_base.hpp"
#include "pmt/unreachable.hpp"

namespace pmt::rt {

namespace {
constexpr auto is_ignored_tag(ClauseBase::Tag tag_) -> bool {
 return tag_ == ClauseBase::Tag::NegativeLookahead || tag_ == ClauseBase::Tag::Epsilon;
}

constexpr auto is_parent_tag(ClauseBase::Tag tag_) -> bool {
 switch (tag_) {
  case ClauseBase::Tag::Sequence:
  case ClauseBase::Tag::Choice:
  case ClauseBase::Tag::OneOrMore:
  case ClauseBase::Tag::Identifier:
   return true;
  case ClauseBase::Tag::NegativeLookahead:
  case ClauseBase::Tag::Epsilon:
  case ClauseBase::Tag::CharsetLiteral:
   return false;
 }
 return false;
}

auto make_string_node(std::string_view input_, MemoTable::Key const& key_, MemoTable::Match const& match_) -> Ast::UniqueHandle {
 Ast::UniqueHandle node = Ast::construct(Ast::Tag::String, AstId::IdDefault);
 node->set_string(input_.substr(key_._position, match_._length));
 return node;
}

auto find_start_match_index_if_success(MemoTable const& memo_table_, std::string_view input_, PikaTablesBase const& pika_program_) -> MemoTable::IndexType {
 ClauseBase const& start_clause = pika_program_.fetch_clause(0);
 MemoTable::IndexType const start_match_index = memo_table_.find(MemoTable::Key{._clause = &start_clause, ._position = 0}, input_, pika_program_);
 if (start_match_index == MemoTable::MemoIndexMatchNotFound) {
  return MemoTable::MemoIndexMatchNotFound;
 }
 if (memo_table_.get_match_length_by_index(start_match_index) != input_.size()) {
  return MemoTable::MemoIndexMatchNotFound;
 }
 return start_match_index;
}

void process_and_add_child(Ast& parent_, Ast::UniqueHandle child_, MemoTable::Match const& child_match_, MemoTable const& memo_table_, PikaTablesBase const& pika_program_) {
 if (!child_) {
  return;
 }

 MemoTable::Key const& child_key = memo_table_.get_key_by_index(child_match_._key_index);
 ClauseBase::Tag const child_tag = child_key._clause->get_tag();
 switch (child_tag) {
  case ClauseBase::Tag::Sequence:
  case ClauseBase::Tag::Choice:
  case ClauseBase::Tag::OneOrMore:
   parent_.give_child_at_back(std::move(child_));
   parent_.unpack(parent_.get_children_size() - 1);
   break;
  case ClauseBase::Tag::CharsetLiteral:
   parent_.give_child_at_back(std::move(child_));
   break;
  case ClauseBase::Tag::Identifier: {
   RuleParametersBase const& rule_parameters = pika_program_.fetch_rule_parameters(child_key._clause->get_rule_id());
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
   break;
  case ClauseBase::Tag::Epsilon:
   break;
  default:
   pmt::unreachable();
 }
}

auto build_ast(MemoTable::Match const& root_match_, MemoTable const& memo_table_, std::string_view input_, PikaTablesBase const& pika_program_) -> Ast::UniqueHandle {
 MemoTable::Key const& root_key = memo_table_.get_key_by_index(root_match_._key_index);
 ClauseBase::Tag const root_tag = root_key._clause->get_tag();
 if (is_ignored_tag(root_tag)) {
  return nullptr;
 }
 if (root_tag == ClauseBase::Tag::CharsetLiteral) {
  return make_string_node(input_, root_key, root_match_);
 }
 assert(is_parent_tag(root_tag));

 struct Frame {
  MemoTable::Match const* _match;
  size_t _next_child_idx;
  Ast::UniqueHandle _ast_node;
 };

 std::vector<Frame> stack;
 stack.push_back(Frame{&root_match_, 0, Ast::construct(Ast::Tag::Parent, AstId::IdUninitialized)});

 Ast::UniqueHandle result;

 while (!stack.empty()) {
  Frame& frame = stack.back();
  MemoTable::Match const& match = *frame._match;

  if (frame._next_child_idx >= match._matching_subclauses.size()) {
   Ast::UniqueHandle completed = std::move(frame._ast_node);
   stack.pop_back();
   if (stack.empty()) {
    result = std::move(completed);
    break;
   }
   Frame& parent = stack.back();
   process_and_add_child(*parent._ast_node, std::move(completed), match, memo_table_, pika_program_);
   continue;
  }

  MemoTable::IndexType const child_match_index = match._matching_subclauses[frame._next_child_idx++];
  if (child_match_index == MemoTable::MemoIndexMatchNotFound || child_match_index == MemoTable::MemoIndexMatchZeroLength) {
   continue;
  }

  MemoTable::Match const& child_match = memo_table_.get_match_by_index(child_match_index);
  MemoTable::Key const& child_key = memo_table_.get_key_by_index(child_match._key_index);
  ClauseBase::Tag const child_tag = child_key._clause->get_tag();
  if (is_parent_tag(child_tag)) {
   stack.push_back(Frame{&child_match, 0, Ast::construct(Ast::Tag::Parent, AstId::IdUninitialized)});
  } else if (child_tag == ClauseBase::Tag::CharsetLiteral) {
   process_and_add_child(*frame._ast_node, make_string_node(input_, child_key, child_match), child_match, memo_table_, pika_program_);
  } else if (!is_ignored_tag(child_tag)) {
   pmt::unreachable();
  }
 }

 return result;
}

}  // namespace

auto PikaParser::memo_table_to_ast(MemoTable const& memo_table_, std::string_view input_, PikaTablesBase const& pika_program_) -> Ast::UniqueHandle {
 MemoTable::IndexType const start_match_index = find_start_match_index_if_success(memo_table_, input_, pika_program_);
 if (start_match_index == MemoTable::MemoIndexMatchNotFound) {
  return nullptr;
 }

 Ast::UniqueHandle ast_root = Ast::construct(Ast::Tag::Parent, AstId::IdRoot);

 if (start_match_index != MemoTable::MemoIndexMatchZeroLength) {
  MemoTable::Match const& start_match = memo_table_.get_match_by_index(start_match_index);
  process_and_add_child(*ast_root, build_ast(start_match, memo_table_, input_, pika_program_), start_match, memo_table_, pika_program_);
 }

 return ast_root;
}

}  // namespace pmt::rt
