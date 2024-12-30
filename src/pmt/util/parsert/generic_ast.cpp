#include "pmt/util/parsert/generic_ast.hpp"

#include "pmt/asserts.hpp"

#include <cassert>
#include <iterator>
#include <queue>
#include <stack>

namespace pmt::util::parsert {

void GenericAst::UniqueHandleDeleter::operator()(GenericAst* self_) const {
  destruct(self_);
}

void GenericAst::destruct(GenericAst* self_) {
  std::stack<GenericAst*> stack;
  stack.push(self_);

  while (!stack.empty()) {
    std::unique_ptr<GenericAst> node{stack.top()};
    stack.pop();

    if (node->get_tag() == Tag::Children) {
      for (size_t i = 0; i < node->get_children_size(); ++i) {
        stack.push(node->get_child_at(i));
      }
    }
  }
}

auto GenericAst::construct(Tag tag_, GenericId::IdType id_) -> UniqueHandle {
  return UniqueHandle{new GenericAst{tag_, id_}};
}

auto GenericAst::clone(GenericAst const& other_) -> UniqueHandle {
  UniqueHandle result = construct(other_.get_tag());

  std::stack<std::pair<GenericAst const*, GenericAst*>> stack;
  stack.push({&other_, result.get()});

  while (!stack.empty()) {
    auto [src, dst] = stack.top();
    stack.pop();

    dst->set_id(src->get_id());

    if (src->get_tag() == Tag::String) {
      dst->set_string(src->get_string());
    } else {
      for (size_t i = 0; i < src->get_children_size(); ++i) {
        UniqueHandle child = construct(src->get_child_at(i)->get_tag());
        dst->give_child_at(i, std::move(child));
        stack.push({src->get_child_at(i), dst->get_child_at(i)});
      }
    }
  }

  return result;
}

void GenericAst::swap(GenericAst& lhs_, GenericAst& rhs_) {
  std::swap(lhs_._data, rhs_._data);
}

auto GenericAst::get_id() const -> GenericId::IdType {
  return _id;
}

void GenericAst::set_id(GenericId::IdType id_) {
  _id = id_;
}

auto GenericAst::get_tag() const -> Tag {
  if (std::holds_alternative<StringType>(_data))
    return Tag::String;
  else if (std::holds_alternative<ChildrenType>(_data))
    return Tag::Children;
  else
    pmt_unreachable();
}

auto GenericAst::get_string() -> StringType& {
  assert(get_tag() == Tag::String);
  return *std::get_if<StringType>(&_data);
}

auto GenericAst::get_string() const -> StringType const& {
  assert(get_tag() == Tag::String);
  return *std::get_if<StringType>(&_data);
}

void GenericAst::set_string(StringType string_) {
  assert(get_tag() == Tag::String);
  _data = std::move(string_);
}

auto GenericAst::get_children_size() const -> std::size_t {
  assert(get_tag() == Tag::Children);
  return std::get_if<ChildrenType>(&_data)->size();
}

auto GenericAst::get_child_at(size_t index_) -> GenericAst* {
  assert(get_tag() == Tag::Children);
  return (index_ >= get_children_size()) ? nullptr : (*std::get_if<ChildrenType>(&_data))[index_];
}

auto GenericAst::get_child_at(size_t index_) const -> GenericAst const* {
  assert(get_tag() == Tag::Children);
  return (index_ >= get_children_size()) ? nullptr : (*std::get_if<ChildrenType>(&_data))[index_];
}

auto GenericAst::get_child_at_front() -> GenericAst* {
  return get_child_at(0);
}

auto GenericAst::get_child_at_front() const -> GenericAst const* {
  return get_child_at(0);
}

auto GenericAst::get_child_at_back() -> GenericAst* {
  return get_child_at(get_children_size() - 1);
}

auto GenericAst::get_child_at_back() const -> GenericAst const* {
  return get_child_at(get_children_size() - 1);
}

auto GenericAst::take_child_at(size_t index_) -> UniqueHandle {
  assert(get_tag() == Tag::Children);
  assert(index_ < get_children_size());
  ChildrenType& children = *std::get_if<ChildrenType>(&_data);
  UniqueHandle result{children[index_]};
  children.erase(std::next(children.begin(), index_));
  return result;
}

void GenericAst::give_child_at(size_t index_, UniqueHandle child_) {
  assert(get_tag() == Tag::Children);
  assert(index_ <= get_children_size());
  ChildrenType& children = *std::get_if<ChildrenType>(&_data);
  children.insert(std::next(children.begin(), index_), child_.release());
}

auto GenericAst::take_child_at_front() -> UniqueHandle {
  return take_child_at(0);
}

void GenericAst::give_child_at_front(UniqueHandle child_) {
  give_child_at(0, std::move(child_));
}

auto GenericAst::take_child_at_back() -> UniqueHandle {
  return take_child_at(get_children_size() - 1);
}

void GenericAst::give_child_at_back(UniqueHandle child_) {
  give_child_at(get_children_size(), std::move(child_));
}

void GenericAst::merge() {
  UniqueHandle result = construct(Tag::String);

  std::queue<GenericAst*> queue;
  queue.push(this);
  while (!queue.empty()) {
    auto* node = queue.front();
    queue.pop();
    if (node->get_tag() == Tag::String) {
      result->get_string() += node->get_string();
    } else {
      for (size_t i = 0; i < node->get_children_size(); ++i) {
        queue.push(node->get_child_at(i));
      }
    }
  }

  swap(*this, *result);
}

void GenericAst::unpack(size_t index_) {
  if (get_tag() != Tag::Children) {
    return;
  }

  ChildrenType& children = *std::get_if<ChildrenType>(&_data);

  if (children[index_]->get_tag() != Tag::Children) {
    return;
  }

  GenericAst::UniqueHandle child = take_child_at(index_);

  while (child->get_children_size() != 0) {
    give_child_at(index_++, child->take_child_at(0));
  }
}

GenericAst::GenericAst(Tag tag_, GenericId::IdType id_)
 : _data{[tag_]() -> std::variant<StringType, ChildrenType> {
   switch (tag_) {
     case Tag::String:
       return StringType{};
     case Tag::Children:
       return ChildrenType{};
     default:
       pmt_unreachable();
   }
 }()}
 , _id(id_) {
}

}  // namespace pmt::util::parsert