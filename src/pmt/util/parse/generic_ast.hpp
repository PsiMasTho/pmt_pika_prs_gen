#pragma once

#include <cstdint>
#include <memory>
#include <string>
#include <variant>
#include <vector>

namespace pmt::util::parse {

class GenericAst {
 public:
  using IdType = std::uint64_t;
  using TokenType = std::string;
  using ChildrenType = std::vector<GenericAst*>;

  enum IdConstants : IdType {
    UninitializedId = -1ULL,
    DefaultId = -2ULL,
  };

  enum class Tag {
    Token,
    Children,
  };

  class UniqueHandleDeleter {
   public:
    void operator()(GenericAst* self_) const;
  };

  static void destruct(GenericAst* self_);
  using UniqueHandle = std::unique_ptr<GenericAst, UniqueHandleDeleter>;
  static auto construct(Tag tag_) -> UniqueHandle;

  static auto clone(GenericAst const& other_) -> UniqueHandle;

  static void swap(GenericAst& lhs_, GenericAst& rhs_);

  auto get_id() const -> IdType;
  void set_id(IdType id_);

  auto get_tag() const -> Tag;

  auto get_token() -> TokenType&;
  auto get_token() const -> TokenType const&;
  void set_token(TokenType token_);

  auto get_children_size() const -> size_t;
  auto get_child_at(size_t index_) -> GenericAst*;
  auto get_child_at(size_t index_) const -> GenericAst const*;

  auto take_child_at(size_t index_) -> UniqueHandle;
  void give_child_at(size_t index_, UniqueHandle child_);

  // Merge the children into a token
  void merge();
  // Unpack the child's children into this node
  void unpack(size_t index_);

 private:
  explicit GenericAst(Tag tag_);

  std::variant<TokenType, ChildrenType> _data;
  IdType _id = IdConstants::UninitializedId;
};

}  // namespace pmt::util::parse