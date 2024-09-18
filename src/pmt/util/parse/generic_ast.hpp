#pragma once

#include <cstdint>
#include <span>
#include <string_view>
#include <variant>
#include <vector>

namespace pmt::util::parse {

class generic_ast_base {
 public:
  using id_type = std::uint64_t;

  static const id_type UNINITIALIZED_ID;

  auto get_id() const -> id_type;
  void set_id(id_type id_);

 private:
  id_type _id = UNINITIALIZED_ID;
};

template <typename CHAR_TYPE_>
class generic_ast : public generic_ast_base {
 public:
  static auto construct() -> generic_ast*;
  static void destruct(generic_ast* self_);

  auto is_token() const -> bool;
  auto is_children() const -> bool;

  auto get_token() const -> std::basic_string_view<CHAR_TYPE_>;
  auto get_children() -> std::span<generic_ast*>;
  auto get_children() const -> std::span<generic_ast const*>;

  auto get_children_size() const -> std::size_t;
  auto get_child_at(std::size_t index_) -> generic_ast*;
  auto get_child_at(std::size_t index_) const -> generic_ast const*;

  void erase_child_at(std::size_t index_);
  void clear_children();

  // These begin the variant lifetimes
  void insert_child_at(std::size_t index_, generic_ast* child_);
  void set_token(std::basic_string_view<CHAR_TYPE_> token_);

 private:
  generic_ast() = default;

  using token_type = std::basic_string_view<CHAR_TYPE_>;
  using children_type = std::vector<generic_ast*>;

  std::variant<std::monostate, children_type, token_type> _data;
};

}  // namespace pmt::util::parse

#include "pmt/util/parse/generic_ast-inl.hpp"