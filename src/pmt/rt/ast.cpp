#include "pmt/rt/ast.hpp"

#include "pmt/unreachable.hpp"

#include <cassert>
#include <iterator>
#include <vector>

namespace pmt::rt {

void Ast::UniqueHandleDeleter::operator()(Ast* self_) const {
 destruct(self_);
}

Ast::Ast(Tag tag_, AstId::IdType id_)
 : _data{[tag_]() -> std::variant<StringType, ParentType> {
  switch (tag_) {
   case Tag::String:
    return StringType{};
   case Tag::Parent:
    return ParentType{};
   default:
    pmt::unreachable();
  }
 }()}
 , _id(id_) {
}

void Ast::destruct(Ast* self_) {
 std::vector<Ast*> pending;
 pending.push_back(self_);

 while (!pending.empty()) {
  std::unique_ptr<Ast> node{pending.back()};
  pending.pop_back();

  if (node->get_tag() == Tag::Parent) {
   for (size_t i = 0; i < node->get_children_size(); ++i) {
    pending.push_back(node->get_child_at(i));
   }
  }
 }
}

auto Ast::construct(Tag tag_, AstId::IdType id_) -> UniqueHandle {
 return UniqueHandle{new Ast{tag_, id_}};
}

auto Ast::clone(Ast const& self_) -> UniqueHandle {
 UniqueHandle result = construct(self_.get_tag());

 std::vector<std::pair<Ast const*, Ast*>> pending;
 pending.push_back({&self_, result.get()});

 while (!pending.empty()) {
  auto [src, dst] = pending.back();
  pending.pop_back();

  dst->set_id(src->get_id());

  if (src->get_tag() == Tag::String) {
   dst->set_string(src->get_string());
  } else {
   for (size_t i = 0; i < src->get_children_size(); ++i) {
    UniqueHandle child = construct(src->get_child_at(i)->get_tag());
    dst->give_child_at(i, std::move(child));
    pending.push_back({src->get_child_at(i), dst->get_child_at(i)});
   }
  }
 }

 return result;
}

void Ast::swap(Ast& lhs_, Ast& rhs_) {
 std::swap(lhs_._data, rhs_._data);
 std::swap(lhs_._id, rhs_._id);
}

auto Ast::get_id() const -> AstId::IdType {
 return _id;
}

void Ast::set_id(AstId::IdType id_) {
 _id = id_;
}

auto Ast::get_tag() const -> Tag {
 if (std::holds_alternative<StringType>(_data)) {
  return Tag::String;
 } else if (std::holds_alternative<ParentType>(_data)) {
  return Tag::Parent;
 } else {
  pmt::unreachable();
 }
}

auto Ast::get_string() -> StringType& {
 assert(get_tag() == Tag::String);
 return *std::get_if<StringType>(&_data);
}

auto Ast::get_string() const -> StringType const& {
 assert(get_tag() == Tag::String);
 return *std::get_if<StringType>(&_data);
}

void Ast::set_string(StringViewType string_view_) {
 assert(get_tag() == Tag::String);
 _data = StringType{string_view_};
}

auto Ast::get_children_size() const -> size_t {
 assert(get_tag() == Tag::Parent);
 return std::get_if<ParentType>(&_data)->size();
}

auto Ast::get_child_at(size_t index_) -> Ast* {
 assert(get_tag() == Tag::Parent);
 return (index_ >= get_children_size()) ? nullptr : (*std::get_if<ParentType>(&_data))[index_];
}

auto Ast::get_child_at(size_t index_) const -> Ast const* {
 assert(get_tag() == Tag::Parent);
 return (index_ >= get_children_size()) ? nullptr : (*std::get_if<ParentType>(&_data))[index_];
}

auto Ast::get_child_at_front() -> Ast* {
 return get_child_at(0);
}

auto Ast::get_child_at_front() const -> Ast const* {
 return get_child_at(0);
}

auto Ast::get_child_at_back() -> Ast* {
 return get_child_at(get_children_size() - 1);
}

auto Ast::get_child_at_back() const -> Ast const* {
 return get_child_at(get_children_size() - 1);
}

auto Ast::take_child_at(size_t index_) -> UniqueHandle {
 assert(get_tag() == Tag::Parent);
 assert(index_ < get_children_size());
 ParentType& children = *std::get_if<ParentType>(&_data);
 UniqueHandle result{children[index_]};
 children.erase(std::next(children.begin(), index_));
 return result;
}

void Ast::give_child_at(size_t index_, UniqueHandle child_) {
 assert(get_tag() == Tag::Parent);
 assert(index_ <= get_children_size());
 ParentType& children = *std::get_if<ParentType>(&_data);
 children.insert(std::next(children.begin(), index_), child_.release());
}

auto Ast::take_child_at_front() -> UniqueHandle {
 return take_child_at(0);
}

void Ast::give_child_at_front(UniqueHandle child_) {
 give_child_at(0, std::move(child_));
}

auto Ast::take_child_at_back() -> UniqueHandle {
 return take_child_at(get_children_size() - 1);
}

void Ast::give_child_at_back(UniqueHandle child_) {
 give_child_at(get_children_size(), std::move(child_));
}

void Ast::merge() {
 UniqueHandle result = construct(Tag::String, get_id());

 std::vector<Ast*> stack;
 stack.push_back(this);
 while (!stack.empty()) {
  auto* node = stack.back();
  stack.pop_back();
  if (node->get_tag() == Tag::String) {
   result->get_string() += node->get_string();
  } else {
   for (size_t i = node->get_children_size(); i--;) {
    stack.push_back(node->get_child_at(i));
   }
  }
 }

 swap(*this, *result);
}

void Ast::unpack(size_t index_) {
 if (get_tag() != Tag::Parent) {
  return;
 }

 ParentType& children = *std::get_if<ParentType>(&_data);

 if (children[index_]->get_tag() != Tag::Parent) {
  return;
 }

 Ast::UniqueHandle child = take_child_at(index_);

 while (child->get_children_size() != 0) {
  give_child_at(index_++, child->take_child_at(0));
 }
}

}  // namespace pmt::rt
