#pragma once

#include "pmt/base/interval_set.hpp"
#include "pmt/parser/grammar/repetition_range.hpp"
#include "pmt/util/sm/primitives.hpp"

#include <deque>
#include <memory>
#include <string>
#include <variant>
#include <vector>

namespace pmt::parser::grammar {

class RuleParameters {
public:
 static inline bool const MERGE_DEFAULT = false;
 static inline bool const UNPACK_DEFAULT = false;

 std::string _display_name;
 std::string _id_string;
 bool _merge : 1 = MERGE_DEFAULT;
 bool _unpack : 1 = UNPACK_DEFAULT;
};

class RuleExpression {
public:
 // -$ Types / Constants $-
 class UniqueHandleDeleter {
 public:
  void operator()(RuleExpression* self_) const;
 };

 using UniqueHandle = std::unique_ptr<RuleExpression, UniqueHandleDeleter>;

private:
 // recursive types
 using SequenceType = std::deque<RuleExpression*>;
 using ChoiceType = std::deque<RuleExpression*>;
 using HiddenType = RuleExpression*;
 using RepetitionType = std::pair<RuleExpression*, RepetitionRange>;

public:
 // non recursive types
 using IdentifierType = std::string;
 using LiteralType = std::vector<pmt::base::IntervalSet<pmt::util::sm::SymbolValueType>>;

 enum class Tag {
  Sequence,
  Choice,
  Hidden,
  Identifier,
  Literal,
  Repetition,
 };

private:
 // Note: keep the values of the Tag constants the same as the indices of their types in the variant
 using VariantType = std::variant<SequenceType, ChoiceType, HiddenType, IdentifierType, LiteralType, RepetitionType>;

 // -$ Data $-
 VariantType _data;

 // -$ Functions $-
 // --$ Lifetime $--
 explicit RuleExpression(Tag tag_);

public:
 static void destruct(RuleExpression* self_);
 static auto construct(Tag tag_) -> UniqueHandle;

 static auto clone(RuleExpression const& other_) -> UniqueHandle;

 // --$ Other $--
 auto get_tag() const -> Tag;

 // recursive types getters / setters
 auto get_children_size() const -> size_t;
 auto get_child_at(size_t index_) -> RuleExpression*;
 auto get_child_at(size_t index_) const -> RuleExpression const*;

 auto get_child_at_front() -> RuleExpression*;
 auto get_child_at_front() const -> RuleExpression const*;

 auto get_child_at_back() -> RuleExpression*;
 auto get_child_at_back() const -> RuleExpression const*;

 auto take_child_at(size_t index_) -> UniqueHandle;
 void give_child_at(size_t index_, UniqueHandle child_);

 auto take_child_at_front() -> UniqueHandle;
 void give_child_at_front(UniqueHandle child_);

 auto take_child_at_back() -> UniqueHandle;
 void give_child_at_back(UniqueHandle child_);

 // non recursive types getters / setters
 auto get_identifier() -> IdentifierType&;
 auto get_identifier() const -> IdentifierType const&;
 void set_identifier(IdentifierType identifier_);

 auto get_literal() -> LiteralType&;
 auto get_literal() const -> LiteralType const&;
 void set_literal(LiteralType literal_);

 // repetition specific
 auto get_repetition_range() -> RepetitionRange&;
 auto get_repetition_range() const -> RepetitionRange const&;
 void set_repetition_range(RepetitionRange repetition_range_);

 // misc
 void unpack(size_t idx_);
};

class Rule {
public:
 RuleParameters _parameters;
 RuleExpression::UniqueHandle _definition;
};

}  // namespace pmt::parser::grammar