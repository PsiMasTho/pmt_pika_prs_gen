// clang-format off
#ifdef __INTELLISENSE__
    #include "pmt/util/parse/generic_ast.hpp"
#endif
// clang-format on

#include <cassert>
#include <iterator>
#include <stack>

namespace pmt::util::parse {

template <typename CHAR_TYPE_>
auto generic_ast<CHAR_TYPE_>::construct() -> generic_ast* {
  return new generic_ast();
}

template <typename CHAR_TYPE_>
void generic_ast<CHAR_TYPE_>::destruct(generic_ast* self_) {
  std::stack<generic_ast*> stack;

  stack.push(self_);
  while (!stack.empty()) {
    auto* node = stack.top();
    stack.pop();

    if (node->is_children()) {
      for (auto* child : node->get_children())
        stack.push(child);
    }

    delete node;
  }
}

template <typename CHAR_TYPE_>
auto generic_ast<CHAR_TYPE_>::is_token() const -> bool {
  return std::holds_alternative<token_type>(_data);
}

template <typename CHAR_TYPE_>
auto generic_ast<CHAR_TYPE_>::is_children() const -> bool {
  return std::holds_alternative<children_type>(_data);
}

template <typename CHAR_TYPE_>
auto generic_ast<CHAR_TYPE_>::get_token() const -> std::basic_string_view<CHAR_TYPE_> {
  assert(is_token());
  return *std::get_if<token_type>(&_data);
}

template <typename CHAR_TYPE_>
auto generic_ast<CHAR_TYPE_>::get_children() -> std::span<generic_ast*> {
  assert(is_children());
  return std::span(*std::get_if<children_type>(&_data));
}

template <typename CHAR_TYPE_>
auto generic_ast<CHAR_TYPE_>::get_children() const -> std::span<generic_ast const*> {
  assert(is_children());
  return std::span(*std::get_if<children_type>(&_data));
}

template <typename CHAR_TYPE_>
auto generic_ast<CHAR_TYPE_>::get_children_size() const -> std::size_t {
  assert(is_children());
  return std::get_if<children_type>(&_data)->size();
}

template <typename CHAR_TYPE_>
auto generic_ast<CHAR_TYPE_>::get_child_at(std::size_t index_) -> generic_ast* {
  assert(is_children());
  return std::get_if<children_type>(&_data)[index_];
}

template <typename CHAR_TYPE_>
auto generic_ast<CHAR_TYPE_>::get_child_at(std::size_t index_) const -> generic_ast const* {
  assert(is_children());
  return std::get_if<children_type>(&_data)[index_];
}

template <typename CHAR_TYPE_>
void generic_ast<CHAR_TYPE_>::erase_child_at(std::size_t index_) {
  assert(is_children());
  children_type& children = std::get_if<children_type>(&_data);
  children.erase(std::next(children.begin(), index_));
}

template <typename CHAR_TYPE_>
void generic_ast<CHAR_TYPE_>::clear_children() {
  assert(is_children());
  std::get_if<children_type>(&_data)->clear();
}

template <typename CHAR_TYPE_>
void generic_ast<CHAR_TYPE_>::insert_child_at(std::size_t index_, generic_ast* child_) {
  if (!is_children())
    _data.template emplace<children_type>();

  children_type& children = std::get_if<children_type>(&_data);
  children.insert(std::next(children.begin(), index_), child_);
}

template <typename CHAR_TYPE_>
void generic_ast<CHAR_TYPE_>::set_token(std::basic_string_view<CHAR_TYPE_> token_) {
  if (!is_token())
    _data.template emplace<token_type>();

  _data = token_;
}

}  // namespace pmt::util::parse