#pragma once

#include "pmt/rt/ast_id.hpp"

#include <deque>
#include <memory>
#include <string>
#include <variant>

namespace pmt::rt {

class Ast {
public:
 // -$ Types / Constants $-
 using StringType = std::string;
 using StringViewType = std::basic_string_view<StringType::value_type>;

 enum class Tag {
  String,
  Parent,
 };

 class UniqueHandleDeleter {
 public:
  void operator()(Ast* self_) const;
 };

 using UniqueHandle = std::unique_ptr<Ast, UniqueHandleDeleter>;

private:
 using ParentType = std::deque<Ast*>;

 // -$ Data $-
 std::variant<StringType, ParentType> _data;
 AstId::IdType _id;

 // -$ Functions $-
 // --$ Lifetime $--
 explicit Ast(Tag tag_, AstId::IdType id_ = AstId::IdUninitialized);

public:
 static void destruct(Ast* self_);
 static auto construct(Tag tag_, AstId::IdType id_ = AstId::IdUninitialized) -> UniqueHandle;

 static auto clone(Ast const& self_) -> UniqueHandle;

 // --$ Other $--
 static void swap(Ast& lhs_, Ast& rhs_);

 auto get_id() const -> AstId::IdType;
 void set_id(AstId::IdType id_);

 auto get_tag() const -> Tag;

 auto get_string() -> StringType&;
 auto get_string() const -> StringType const&;
 void set_string(StringViewType string_view_);

 auto get_children_size() const -> size_t;
 auto get_child_at(size_t index_) -> Ast*;
 auto get_child_at(size_t index_) const -> Ast const*;

 auto get_child_at_front() -> Ast*;
 auto get_child_at_front() const -> Ast const*;

 auto get_child_at_back() -> Ast*;
 auto get_child_at_back() const -> Ast const*;

 auto take_child_at(size_t index_) -> UniqueHandle;
 void give_child_at(size_t index_, UniqueHandle child_);

 auto take_child_at_front() -> UniqueHandle;
 void give_child_at_front(UniqueHandle child_);

 auto take_child_at_back() -> UniqueHandle;
 void give_child_at_back(UniqueHandle child_);

 // Merge the children into a string, keeps the id of *this
 void merge();
 // Unpack the child's children into this node
 void unpack(size_t index_);
};

}  // namespace pmt::rt