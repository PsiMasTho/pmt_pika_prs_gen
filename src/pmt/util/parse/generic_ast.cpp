#include "pmt/util/parse/generic_ast.hpp"

#include "pmt/asserts.hpp"

#include <cassert>
#include <iterator>
#include <queue>
#include <stack>

namespace pmt::util::parse {

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

auto GenericAst::construct(Tag tag_) -> UniqueHandle {
  return UniqueHandle{new GenericAst{tag_}};
}

auto GenericAst::get_id() const -> IdType {
  return _id;
}

void GenericAst::set_id(IdType id_) {
  _id = id_;
}

void GenericAst::swap(GenericAst& lhs_, GenericAst& rhs_) {
  std::swap(lhs_._data, rhs_._data);
}

auto GenericAst::get_tag() const -> Tag {
  if (std::holds_alternative<TokenType>(_data))
    return Tag::Token;
  else if (std::holds_alternative<ChildrenType>(_data))
    return Tag::Children;
  else
    pmt_unreachable();
}

auto GenericAst::get_token() -> TokenType& {
  assert(get_tag() == Tag::Token);
  return *std::get_if<TokenType>(&_data);
}

auto GenericAst::get_token() const -> TokenType const& {
  assert(get_tag() == Tag::Token);
  return *std::get_if<TokenType>(&_data);
}

void GenericAst::set_token(TokenType token_) {
  assert(get_tag() == Tag::Token);
  _data = std::move(token_);
}

auto GenericAst::get_children_size() const -> std::size_t {
  assert(get_tag() == Tag::Children);
  return std::get_if<ChildrenType>(&_data)->size();
}

auto GenericAst::get_child_at(std::size_t index_) -> GenericAst* {
  assert(get_tag() == Tag::Children);
  return (*std::get_if<ChildrenType>(&_data))[index_];
}

auto GenericAst::get_child_at(std::size_t index_) const -> GenericAst const* {
  assert(get_tag() == Tag::Children);
  return (*std::get_if<ChildrenType>(&_data))[index_];
}
auto GenericAst::take_child_at(std::size_t index_) -> UniqueHandle {
  assert(get_tag() == Tag::Children);
  ChildrenType& children = *std::get_if<ChildrenType>(&_data);
  UniqueHandle result{children[index_]};
  children.erase(std::next(children.begin(), index_));
  return result;
}

void GenericAst::give_child_at(std::size_t index_, UniqueHandle child_) {
  assert(get_tag() == Tag::Children);
  ChildrenType& children = *std::get_if<ChildrenType>(&_data);
  children.insert(std::next(children.begin(), index_), child_.release());
}

void GenericAst::merge() {
  UniqueHandle result = construct(Tag::Token);

  std::queue<GenericAst*> queue;
  queue.push(this);
  while (!queue.empty()) {
    auto* node = queue.front();
    queue.pop();
    if (node->get_tag() == Tag::Token) {
      result->get_token() += node->get_token();
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

GenericAst::GenericAst(Tag tag_)
 : _data{[tag_]() -> std::variant<TokenType, ChildrenType> {
   switch (tag_) {
     case Tag::Token:
       return TokenType{};
     case Tag::Children:
       return ChildrenType{};
     default:
       pmt_unreachable();
   }
 }()} {
}

}  // namespace pmt::util::parse