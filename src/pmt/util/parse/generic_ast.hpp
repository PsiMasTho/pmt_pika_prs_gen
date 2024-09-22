#pragma once

#include <cstdint>
#include <memory>
#include <string>
#include <variant>
#include <vector>

#include "pmt/util/text_encoding.hpp"

namespace pmt::util::parse {

class generic_ast_base {
 public:
  using id_type = std::uint64_t;

  enum : id_type {
    UNINITIALIZED_ID = -1ULL,
    DEFAULT_ID = -2ULL,
  };

  enum tag {
    TAG_TOKEN,
    TAG_CHILDREN,
  };

  auto get_id() const -> id_type;
  void set_id(id_type id_);

 private:
  id_type _id = UNINITIALIZED_ID;
};

template <text_encoding ENCODING_>
class generic_ast : public generic_ast_base {
 public:
  using char_type = typename text_encoding_traits<ENCODING_>::code_unit_type;
  using token_type = std::basic_string<char_type>;
  using children_type = std::vector<generic_ast*>;

  static void destruct(generic_ast* self_);
  using unique_handle = std::unique_ptr<generic_ast, decltype(&destruct)>;
  static auto construct(tag tag_) -> unique_handle;

  static void swap(generic_ast& lhs_, generic_ast& rhs_);

  auto get_tag() const -> tag;

  auto get_token() -> token_type&;
  auto get_token() const -> token_type const&;
  void set_token(token_type token_);

  auto get_children_size() const -> std::size_t;
  auto get_child_at(std::size_t index_) -> generic_ast*;
  auto get_child_at(std::size_t index_) const -> generic_ast const*;

  auto take_child_at(std::size_t index_) -> unique_handle;
  void give_child_at(std::size_t index_, unique_handle child_);

  // Merge the children into a token
  void merge();

 private:
  explicit generic_ast(tag tag_);

  std::variant<token_type, children_type> _data;
};

}  // namespace pmt::util::parse

#include "pmt/util/parse/generic_ast-inl.hpp"