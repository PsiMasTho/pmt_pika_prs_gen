#pragma once

#include "pmt/parser/clause_base.hpp"
#include "pmt/parser/grammar/charset_literal.hpp"
#include "pmt/parser/rt/non_terminal.hpp"

#include <deque>
#include <memory>
#include <string>
#include <variant>

namespace pmt::parser::grammar {

class RuleParameters : public pmt::parser::rt::NonTerminal {
public:
 std::string _id_string;
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
 using OneOrMoreType = RuleExpression*;
 using NotFollowedByType = RuleExpression*;

 // non recursive types
 using EpsilonType = std::monostate;

public:
 using IdentifierType = std::string;

private:
 // Note: keep the values of the ClauseBase::Tag constants the same as the indices of their types in the variant
 using VariantType = std::variant<SequenceType, ChoiceType, HiddenType, IdentifierType, CharsetLiteral, OneOrMoreType, NotFollowedByType, EpsilonType>;

 // -$ Data $-
 VariantType _data;

 // -$ Functions $-
 // --$ Lifetime $--
 explicit RuleExpression(ClauseBase::Tag tag_);

public:
 static void destruct(RuleExpression* self_);
 static auto construct(ClauseBase::Tag tag_) -> UniqueHandle;

 static auto clone(RuleExpression const& other_) -> UniqueHandle;

 // --$ Other $--
 auto get_tag() const -> ClauseBase::Tag;

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

 auto get_charset_literal() -> CharsetLiteral&;
 auto get_charset_literal() const -> CharsetLiteral const&;
 void set_charset_literal(CharsetLiteral literal_);

 // misc
 void unpack(size_t idx_);
};

class Rule {
public:
 RuleParameters _parameters;
 RuleExpression::UniqueHandle _definition;
};

}  // namespace pmt::parser::grammar