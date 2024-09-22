// clang-format off
#ifdef __INTELLISENSE__
    #include "pmt/util/parse/generic_ast.hpp"
#endif
// clang-format on

#include <cassert>
#include <iterator>
#include <queue>
#include <stack>

#include "pmt/asserts.hpp"

namespace pmt::util::parse {

template <text_encoding ENCODING_>
void generic_ast<ENCODING_>::destruct(generic_ast* self_) {
  std::stack<generic_ast*> stack;
  stack.push(self_);

  while (!stack.empty()) {
    auto* node = stack.top();
    stack.pop();

    if (node->get_tag() == TAG_CHILDREN) {
      for (size_t i = 0; i < node->get_children_size(); ++i) {
        stack.push(node->get_child_at(i));
      }
    }

    delete node;
  }
}

template <text_encoding ENCODING_>
auto generic_ast<ENCODING_>::construct(tag tag_) -> unique_handle {
  return unique_handle{new generic_ast{tag_}, destruct};
}

template <text_encoding ENCODING_>
void generic_ast<ENCODING_>::swap(generic_ast<ENCODING_>& lhs_, generic_ast<ENCODING_>& rhs_) {
  std::swap(lhs_._data, rhs_._data);
}

template <text_encoding ENCODING_>
auto generic_ast<ENCODING_>::get_tag() const -> tag {
  if (std::holds_alternative<token_type>(_data))
    return TAG_TOKEN;
  else if (std::holds_alternative<children_type>(_data))
    return TAG_CHILDREN;
  else
    pmt_unreachable();
}

template <text_encoding ENCODING_>
auto generic_ast<ENCODING_>::get_token() -> token_type& {
  assert(get_tag() == TAG_TOKEN);
  return *std::get_if<token_type>(&_data);
}

template <text_encoding ENCODING_>
auto generic_ast<ENCODING_>::get_token() const -> token_type const& {
  assert(get_tag() == TAG_TOKEN);
  return *std::get_if<token_type>(&_data);
}

template <text_encoding ENCODING_>
void generic_ast<ENCODING_>::set_token(token_type token_) {
  assert(get_tag() == TAG_TOKEN);
  _data = std::move(token_);
}

template <text_encoding ENCODING_>
auto generic_ast<ENCODING_>::get_children_size() const -> std::size_t {
  assert(get_tag() == TAG_CHILDREN);
  return std::get_if<children_type>(&_data)->size();
}

template <text_encoding ENCODING_>
auto generic_ast<ENCODING_>::get_child_at(std::size_t index_) -> generic_ast* {
  assert(get_tag() == TAG_CHILDREN);
  return (*std::get_if<children_type>(&_data))[index_];
}

template <text_encoding ENCODING_>
auto generic_ast<ENCODING_>::get_child_at(std::size_t index_) const -> generic_ast const* {
  assert(get_tag() == TAG_CHILDREN);
  return (*std::get_if<children_type>(&_data))[index_];
}
template <text_encoding ENCODING_>
auto generic_ast<ENCODING_>::take_child_at(std::size_t index_) -> unique_handle {
  assert(get_tag() == TAG_CHILDREN);
  children_type& children = *std::get_if<children_type>(&_data);
  unique_handle result{children[index_], destruct};
  children.erase(std::next(children.begin(), index_));
  return result;
}

template <text_encoding ENCODING_>
void generic_ast<ENCODING_>::give_child_at(std::size_t index_, unique_handle child_) {
  assert(get_tag() == TAG_CHILDREN);
  children_type& children = *std::get_if<children_type>(&_data);
  children.insert(std::next(children.begin(), index_), child_.release());
}

template <text_encoding ENCODING_>
void generic_ast<ENCODING_>::merge() {
  unique_handle result = construct(TAG_TOKEN);

  std::queue<generic_ast*> queue;
  queue.push(this);
  while (!queue.empty()) {
    auto* node = queue.front();
    queue.pop();
    if (node->get_tag() == TAG_TOKEN) {
      result->get_token() += node->get_token();
    } else {
      for (size_t i = 0; i < node->get_children_size(); ++i) {
        queue.push(node->get_child_at(i));
      }
    }
  }

  swap(*this, *result);
}

template <text_encoding ENCODING_>
generic_ast<ENCODING_>::generic_ast(tag tag_)
 : _data{[tag_]() -> std::variant<token_type, children_type> {
     switch (tag_) {
       case TAG_TOKEN:
         return token_type{};
       case TAG_CHILDREN:
         return children_type{};
       default:
         pmt_unreachable();
     }
   }()} {
}

}  // namespace pmt::util::parse
