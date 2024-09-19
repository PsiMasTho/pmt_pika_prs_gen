#pragma once

#include <cstdint>
#include <memory>
#include <span>
#include <string>
#include <variant>
#include <vector>

namespace pmt::util::parse {

class generic_ast_base {
 public:
  using id_type = std::uint64_t;

  enum : id_type {
    UNINITIALIZED_ID = -1ULL,
    ANONYMOUS_ID = -2ULL,
  };

  auto get_id() const -> id_type;
  void set_id(id_type id_);

 private:
  id_type _id = UNINITIALIZED_ID;
};

template <typename CHAR_TYPE_>
class generic_ast : public generic_ast_base {
 public:
  using token_type = std::basic_string<CHAR_TYPE_>;
  using children_type = std::vector<generic_ast*>;

  static auto construct() -> generic_ast*;
  static void destruct(generic_ast* self_);

  using unique_handle = std::unique_ptr<generic_ast, decltype(&destruct)>;

  auto is_token() const -> bool;
  auto is_children() const -> bool;

  auto get_token() const -> token_type;
  auto get_children() -> std::span<generic_ast*>;
  auto get_children() const -> std::span<generic_ast const*>;

  auto get_children_size() const -> std::size_t;
  auto get_child_at(std::size_t index_) -> generic_ast*;
  auto get_child_at(std::size_t index_) const -> generic_ast const*;

  //-- Child ownership ---------------------------------------------------------
  auto take_child_at(std::size_t index_) -> unique_handle;
  void delete_child_at(std::size_t index_);

  //-- Lifetime init -----------------------------------------------------------
  void insert_child_at(std::size_t index_, generic_ast* child_);
  void set_token(token_type token_);

  // Merge the children into a token
  void merge();

 private:
  generic_ast() = default;

  std::variant<std::monostate, children_type, token_type> _data;
};

}  // namespace pmt::util::parse

#include "pmt/util/parse/generic_ast-inl.hpp"