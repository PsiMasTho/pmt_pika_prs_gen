#include "pmt/parser/grammar/rule.hpp"

#include "pmt/asserts.hpp"

namespace pmt::parser::grammar {
namespace {}

void RuleExpression::UniqueHandleDeleter::operator()(RuleExpression* self_) const {
 destruct(self_);
}

RuleExpression::RuleExpression(ClauseBase::Tag tag_)
 : _data{[tag_]() -> VariantType {
  switch (tag_) {
   case ClauseBase::Tag::Sequence:
    return VariantType(std::in_place_index<static_cast<size_t>(ClauseBase::Tag::Sequence)>);
   case ClauseBase::Tag::Choice:
    return VariantType(std::in_place_index<static_cast<size_t>(ClauseBase::Tag::Choice)>);
   case ClauseBase::Tag::Hidden:
    return VariantType(std::in_place_index<static_cast<size_t>(ClauseBase::Tag::Hidden)>, nullptr);
   case ClauseBase::Tag::Identifier:
    return IdentifierType{};
   case ClauseBase::Tag::Literal:
    return CharsetLiteral{};
   case ClauseBase::Tag::OneOrMore:
    return VariantType(std::in_place_index<static_cast<size_t>(ClauseBase::Tag::OneOrMore)>, nullptr);
   case ClauseBase::Tag::NotFollowedBy:
    return VariantType(std::in_place_index<static_cast<size_t>(ClauseBase::Tag::NotFollowedBy)>, nullptr);
   case ClauseBase::Tag::Epsilon:
    return VariantType(std::in_place_index<static_cast<size_t>(ClauseBase::Tag::Epsilon)>);
   default:
    pmt::unreachable();
  }
 }()} {
}

void RuleExpression::destruct(RuleExpression* self_) {
 std::vector<RuleExpression*> pending;
 pending.push_back(self_);

 while (!pending.empty()) {
  std::unique_ptr<RuleExpression> node{pending.back()};
  pending.pop_back();

  for (size_t i = 0; i < node->get_children_size(); ++i) {
   pending.push_back(node->get_child_at(i));
  }
 }
}

auto RuleExpression::construct(ClauseBase::Tag tag_) -> UniqueHandle {
 return UniqueHandle{new RuleExpression{tag_}};
}

auto RuleExpression::clone(RuleExpression const& other_) -> UniqueHandle {
 UniqueHandle result = construct(other_.get_tag());

 std::vector<std::pair<RuleExpression const*, RuleExpression*>> pending;
 pending.push_back({&other_, result.get()});

 while (!pending.empty()) {
  auto [src, dst] = pending.back();
  pending.pop_back();

  switch (src->get_tag()) {
   case ClauseBase::Tag::OneOrMore:
   case ClauseBase::Tag::NotFollowedBy:
   case ClauseBase::Tag::Sequence:
   case ClauseBase::Tag::Choice:
   case ClauseBase::Tag::Hidden: {
    for (size_t i = 0; i < src->get_children_size(); ++i) {
     UniqueHandle child = construct(src->get_child_at(i)->get_tag());
     dst->give_child_at(i, std::move(child));
     pending.push_back({src->get_child_at(i), dst->get_child_at(i)});
    }
   } break;
   case ClauseBase::Tag::Identifier: {
    dst->set_identifier(src->get_identifier());
   } break;
   case ClauseBase::Tag::Literal: {
    dst->set_charset_literal(src->get_charset_literal());
   } break;
   case ClauseBase::Tag::Epsilon: {
   } break;
  }
 }

 return result;
}

auto RuleExpression::get_tag() const -> ClauseBase::Tag {
 return static_cast<ClauseBase::Tag>(_data.index());
}

auto RuleExpression::get_children_size() const -> size_t {
 switch (get_tag()) {
  case ClauseBase::Tag::Sequence:
   return std::get<static_cast<size_t>(ClauseBase::Tag::Sequence)>(_data).size();
  case ClauseBase::Tag::Choice:
   return std::get<static_cast<size_t>(ClauseBase::Tag::Choice)>(_data).size();
  case ClauseBase::Tag::Hidden:
   return std::get<static_cast<size_t>(ClauseBase::Tag::Hidden)>(_data) == nullptr ? 0 : 1;
  case ClauseBase::Tag::OneOrMore:
   return std::get<static_cast<size_t>(ClauseBase::Tag::OneOrMore)>(_data) == nullptr ? 0 : 1;
  case ClauseBase::Tag::NotFollowedBy:
   return std::get<static_cast<size_t>(ClauseBase::Tag::NotFollowedBy)>(_data) == nullptr ? 0 : 1;
  default:
   return 0;  // Cannot have children
 }
}

auto RuleExpression::get_child_at(size_t index_) -> RuleExpression* {
 if (index_ >= get_children_size()) {
  return nullptr;
 }

 switch (get_tag()) {
  case ClauseBase::Tag::Sequence:
   return std::get<static_cast<size_t>(ClauseBase::Tag::Sequence)>(_data)[index_];
  case ClauseBase::Tag::Choice:
   return std::get<static_cast<size_t>(ClauseBase::Tag::Choice)>(_data)[index_];
  case ClauseBase::Tag::Hidden:
   return std::get<static_cast<size_t>(ClauseBase::Tag::Hidden)>(_data);
  case ClauseBase::Tag::OneOrMore:
   return std::get<static_cast<size_t>(ClauseBase::Tag::OneOrMore)>(_data);
  case ClauseBase::Tag::NotFollowedBy:
   return std::get<static_cast<size_t>(ClauseBase::Tag::NotFollowedBy)>(_data);
  default:
   pmt::unreachable();
 }
}

auto RuleExpression::get_child_at(size_t index_) const -> RuleExpression const* {
 if (index_ >= get_children_size()) {
  return nullptr;
 }

 switch (get_tag()) {
  case ClauseBase::Tag::Sequence:
   return std::get<static_cast<size_t>(ClauseBase::Tag::Sequence)>(_data)[index_];
  case ClauseBase::Tag::Choice:
   return std::get<static_cast<size_t>(ClauseBase::Tag::Choice)>(_data)[index_];
  case ClauseBase::Tag::Hidden:
   return std::get<static_cast<size_t>(ClauseBase::Tag::Hidden)>(_data);
  case ClauseBase::Tag::OneOrMore:
   return std::get<static_cast<size_t>(ClauseBase::Tag::OneOrMore)>(_data);
  case ClauseBase::Tag::NotFollowedBy:
   return std::get<static_cast<size_t>(ClauseBase::Tag::NotFollowedBy)>(_data);
  default:
   pmt::unreachable();
 }
}

auto RuleExpression::get_child_at_front() -> RuleExpression* {
 return get_child_at(0);
}

auto RuleExpression::get_child_at_front() const -> RuleExpression const* {
 return get_child_at(0);
}

auto RuleExpression::get_child_at_back() -> RuleExpression* {
 return get_child_at(get_children_size() - 1);
}

auto RuleExpression::get_child_at_back() const -> RuleExpression const* {
 return get_child_at(get_children_size() - 1);
}

auto RuleExpression::take_child_at(size_t index_) -> UniqueHandle {
 UniqueHandle ret{get_child_at(index_)};

 switch (get_tag()) {
  case ClauseBase::Tag::Sequence: {
   SequenceType& container = std::get<static_cast<size_t>(ClauseBase::Tag::Sequence)>(_data);
   container.erase(std::next(container.begin(), index_));
  } break;
  case ClauseBase::Tag::Choice: {
   SequenceType& container = std::get<static_cast<size_t>(ClauseBase::Tag::Choice)>(_data);
   container.erase(std::next(container.begin(), index_));
  } break;
  case ClauseBase::Tag::Hidden:
   std::get<static_cast<size_t>(ClauseBase::Tag::Hidden)>(_data) = nullptr;
   break;
  case ClauseBase::Tag::OneOrMore:
   std::get<static_cast<size_t>(ClauseBase::Tag::OneOrMore)>(_data) = nullptr;
   break;
  case ClauseBase::Tag::NotFollowedBy:
   std::get<static_cast<size_t>(ClauseBase::Tag::NotFollowedBy)>(_data) = nullptr;
   break;
  default:
   pmt::unreachable();
 }
 return ret;
}

void RuleExpression::give_child_at(size_t index_, UniqueHandle child_) {
 assert(index_ <= get_children_size());

 switch (get_tag()) {
  case ClauseBase::Tag::Sequence: {
   SequenceType& container = std::get<static_cast<size_t>(ClauseBase::Tag::Sequence)>(_data);
   container.insert(std::next(container.begin(), index_), child_.release());
  } break;
  case ClauseBase::Tag::Choice: {
   SequenceType& container = std::get<static_cast<size_t>(ClauseBase::Tag::Choice)>(_data);
   container.insert(std::next(container.begin(), index_), child_.release());
  } break;
  case ClauseBase::Tag::Hidden:
   std::get<static_cast<size_t>(ClauseBase::Tag::Hidden)>(_data) = child_.release();
   break;
  case ClauseBase::Tag::OneOrMore:
   std::get<static_cast<size_t>(ClauseBase::Tag::OneOrMore)>(_data) = child_.release();
   break;
  case ClauseBase::Tag::NotFollowedBy:
   std::get<static_cast<size_t>(ClauseBase::Tag::NotFollowedBy)>(_data) = child_.release();
   break;
  default:
   pmt::unreachable();
 }
}

auto RuleExpression::take_child_at_front() -> UniqueHandle {
 return take_child_at(0);
}

void RuleExpression::give_child_at_front(UniqueHandle child_) {
 give_child_at(0, std::move(child_));
}

auto RuleExpression::take_child_at_back() -> UniqueHandle {
 return take_child_at(get_children_size() - 1);
}

void RuleExpression::give_child_at_back(UniqueHandle child_) {
 give_child_at(get_children_size(), std::move(child_));
}

auto RuleExpression::get_identifier() -> IdentifierType& {
 assert(get_tag() == ClauseBase::Tag::Identifier);
 return *std::get_if<static_cast<size_t>(ClauseBase::Tag::Identifier)>(&_data);
}

auto RuleExpression::get_identifier() const -> IdentifierType const& {
 assert(get_tag() == ClauseBase::Tag::Identifier);
 return *std::get_if<static_cast<size_t>(ClauseBase::Tag::Identifier)>(&_data);
}

void RuleExpression::set_identifier(IdentifierType identifier_) {
 _data = std::move(identifier_);
}

auto RuleExpression::get_charset_literal() -> CharsetLiteral& {
 assert(get_tag() == ClauseBase::Tag::Literal);
 return *std::get_if<static_cast<size_t>(ClauseBase::Tag::Literal)>(&_data);
}

auto RuleExpression::get_charset_literal() const -> CharsetLiteral const& {
 assert(get_tag() == ClauseBase::Tag::Literal);
 return *std::get_if<static_cast<size_t>(ClauseBase::Tag::Literal)>(&_data);
}

void RuleExpression::set_charset_literal(CharsetLiteral literal_) {
 _data = std::move(literal_);
}

void RuleExpression::unpack(size_t idx_) {
 if (idx_ >= get_children_size()) {
  return;
 }

 RuleExpression::UniqueHandle child = take_child_at(idx_);

 while (child->get_children_size() != 0) {
  give_child_at(idx_++, child->take_child_at(0));
 }
}

}  // namespace pmt::parser::grammar