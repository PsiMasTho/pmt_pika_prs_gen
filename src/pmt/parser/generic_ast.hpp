#pragma once

#include "pmt/parser/generic_id.hpp"

#include <deque>
#include <memory>
#include <string>
#include <variant>

namespace pmt::parser {

class GenericAst {
public:
 // -$ Types / Constants $-
 using StringType = std::string;

 enum class Tag {
  String,
  Children,
 };

 class UniqueHandleDeleter {
 public:
  void operator()(GenericAst* self_) const;
 };

 using UniqueHandle = std::unique_ptr<GenericAst, UniqueHandleDeleter>;

private:
 using ChildrenType = std::deque<GenericAst*>;

 // -$ Data $-
 std::variant<StringType, ChildrenType> _data;
 GenericId::IdType _id;

 // -$ Functions $-
 // --$ Lifetime $--
 explicit GenericAst(Tag tag_, GenericId::IdType id_ = GenericId::IdUninitialized);

public:
 static void destruct(GenericAst* self_);
 static auto construct(Tag tag_, GenericId::IdType id_ = GenericId::IdUninitialized) -> UniqueHandle;

 static auto clone(GenericAst const& other_) -> UniqueHandle;

 // --$ Other $--
 static void swap(GenericAst& lhs_, GenericAst& rhs_);

 auto get_id() const -> GenericId::IdType;
 void set_id(GenericId::IdType id_);

 auto get_tag() const -> Tag;

 auto get_string() -> StringType&;
 auto get_string() const -> StringType const&;
 void set_string(StringType string_);

 auto get_children_size() const -> size_t;
 auto get_child_at(size_t index_) -> GenericAst*;
 auto get_child_at(size_t index_) const -> GenericAst const*;

 auto get_child_at_front() -> GenericAst*;
 auto get_child_at_front() const -> GenericAst const*;

 auto get_child_at_back() -> GenericAst*;
 auto get_child_at_back() const -> GenericAst const*;

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

}  // namespace pmt::parser