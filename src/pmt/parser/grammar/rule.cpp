#include "pmt/parser/grammar/rule.hpp"

#include "pmt/asserts.hpp"

namespace pmt::parser::grammar {
namespace {}

void RuleExpression::UniqueHandleDeleter::operator()(RuleExpression* self_) const {
 destruct(self_);
}

RuleExpression::RuleExpression(Tag tag_)
 : _data{[tag_]() -> VariantType {
  switch (tag_) {
   case Tag::Sequence:
    return VariantType(std::in_place_index<static_cast<size_t>(Tag::Sequence)>);
   case Tag::Choice:
    return VariantType(std::in_place_index<static_cast<size_t>(Tag::Choice)>);
   case Tag::Hidden:
    return HiddenType{nullptr};
   case Tag::Identifier:
    return IdentifierType{};
   case Tag::Literal:
    return LiteralType{};
   case Tag::Repetition:
    return RepetitionType{nullptr, {}};
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

auto RuleExpression::construct(Tag tag_) -> UniqueHandle {
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
   case Tag::Repetition: {
    dst->set_repetition_range(src->get_repetition_range());
   }
   case Tag::Sequence:
   case Tag::Choice:
   case Tag::Hidden: {
    for (size_t i = 0; i < src->get_children_size(); ++i) {
     UniqueHandle child = construct(src->get_child_at(i)->get_tag());
     dst->give_child_at(i, std::move(child));
     pending.push_back({src->get_child_at(i), dst->get_child_at(i)});
    }
   } break;
   case Tag::Identifier: {
    dst->set_identifier(src->get_identifier());
   } break;
   case Tag::Literal: {
    dst->set_literal(src->get_literal());
   } break;
  }
 }

 return result;
}

auto RuleExpression::get_tag() const -> Tag {
 return static_cast<Tag>(_data.index());
}

auto RuleExpression::get_children_size() const -> size_t {
 switch (get_tag()) {
  case Tag::Sequence:
   return std::get<static_cast<size_t>(Tag::Sequence)>(_data).size();
  case Tag::Choice:
   return std::get<static_cast<size_t>(Tag::Choice)>(_data).size();
  case Tag::Hidden:
   return std::get<static_cast<size_t>(Tag::Hidden)>(_data) == nullptr ? 0 : 1;
  case Tag::Repetition:
   return std::get<static_cast<size_t>(Tag::Repetition)>(_data).first == nullptr ? 0 : 1;
  default:
   return 0;  // Cannot have children
 }
}

auto RuleExpression::get_child_at(size_t index_) -> RuleExpression* {
 if (index_ >= get_children_size()) {
  return nullptr;
 }

 switch (get_tag()) {
  case Tag::Sequence:
   return std::get<static_cast<size_t>(Tag::Sequence)>(_data)[index_];
  case Tag::Choice:
   return std::get<static_cast<size_t>(Tag::Choice)>(_data)[index_];
  case Tag::Hidden:
   return std::get<static_cast<size_t>(Tag::Hidden)>(_data);
  case Tag::Repetition:
   return std::get<static_cast<size_t>(Tag::Repetition)>(_data).first;
  default:
   pmt::unreachable();
 }
}

auto RuleExpression::get_child_at(size_t index_) const -> RuleExpression const* {
 if (index_ >= get_children_size()) {
  return nullptr;
 }

 switch (get_tag()) {
  case Tag::Sequence:
   return std::get<static_cast<size_t>(Tag::Sequence)>(_data)[index_];
  case Tag::Choice:
   return std::get<static_cast<size_t>(Tag::Choice)>(_data)[index_];
  case Tag::Hidden:
   return std::get<static_cast<size_t>(Tag::Hidden)>(_data);
  case Tag::Repetition:
   return std::get<static_cast<size_t>(Tag::Repetition)>(_data).first;
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
  case Tag::Sequence: {
   SequenceType& container = std::get<static_cast<size_t>(Tag::Sequence)>(_data);
   container.erase(std::next(container.begin(), index_));
  } break;
  case Tag::Choice: {
   SequenceType& container = std::get<static_cast<size_t>(Tag::Choice)>(_data);
   container.erase(std::next(container.begin(), index_));
  } break;
  case Tag::Hidden:
   std::get<static_cast<size_t>(Tag::Hidden)>(_data) = nullptr;
   break;
  case Tag::Repetition:
   std::get<static_cast<size_t>(Tag::Repetition)>(_data).first = nullptr;
   break;
  default:
   pmt::unreachable();
 }
 return ret;
}

void RuleExpression::give_child_at(size_t index_, UniqueHandle child_) {
 assert(index_ <= get_children_size());

 switch (get_tag()) {
  case Tag::Sequence: {
   SequenceType& container = std::get<static_cast<size_t>(Tag::Sequence)>(_data);
   container.insert(std::next(container.begin(), index_), child_.release());
  } break;
  case Tag::Choice: {
   SequenceType& container = std::get<static_cast<size_t>(Tag::Choice)>(_data);
   container.insert(std::next(container.begin(), index_), child_.release());
  } break;
  case Tag::Hidden:
   std::get<static_cast<size_t>(Tag::Hidden)>(_data) = child_.release();
   break;
  case Tag::Repetition:
   std::get<static_cast<size_t>(Tag::Repetition)>(_data).first = child_.release();
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
 assert(get_tag() == Tag::Identifier);
 return *std::get_if<static_cast<size_t>(Tag::Identifier)>(&_data);
}

auto RuleExpression::get_identifier() const -> IdentifierType const& {
 assert(get_tag() == Tag::Identifier);
 return *std::get_if<static_cast<size_t>(Tag::Identifier)>(&_data);
}

void RuleExpression::set_identifier(IdentifierType identifier_) {
 _data = std::move(identifier_);
}

auto RuleExpression::get_literal() -> LiteralType& {
 assert(get_tag() == Tag::Literal);
 return *std::get_if<static_cast<size_t>(Tag::Literal)>(&_data);
}

auto RuleExpression::get_literal() const -> LiteralType const& {
 assert(get_tag() == Tag::Literal);
 return *std::get_if<static_cast<size_t>(Tag::Literal)>(&_data);
}

void RuleExpression::set_literal(LiteralType literal_) {
 _data = std::move(literal_);
}

auto RuleExpression::get_repetition_range() -> RepetitionRange& {
 assert(get_tag() == Tag::Repetition);
 return std::get_if<static_cast<size_t>(Tag::Repetition)>(&_data)->second;
}

auto RuleExpression::get_repetition_range() const -> RepetitionRange const& {
 assert(get_tag() == Tag::Repetition);
 return std::get_if<static_cast<size_t>(Tag::Repetition)>(&_data)->second;
}

void RuleExpression::set_repetition_range(RepetitionRange repetition_range_) {
 assert(get_tag() == Tag::Repetition);
 std::get_if<static_cast<size_t>(Tag::Repetition)>(&_data)->second = repetition_range_;
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