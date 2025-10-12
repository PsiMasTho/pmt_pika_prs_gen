#include "pmt/parser/clause_base.hpp"

#include "pmt/asserts.hpp"

namespace pmt::parser {

auto ClauseBase::tag_to_string(Tag tag_) -> std::string {
 switch (tag_) {
  case Tag::Sequence:
   return "Sequence";
  case Tag::Choice:
   return "Choice";
  case Tag::Hidden:
   return "Hidden";
  case Tag::Regular:
   return "Regular";
  case Tag::Identifier:
   return "Identifier";
  case Tag::Literal:
   return "Literal";
  case Tag::OneOrMore:
   return "OneOrMore";
  case Tag::NotFollowedBy:
   return "NotFollowedBy";
  case Tag::Epsilon:
   return "Epsilon";
  default:
   pmt::unreachable();
 }
}

}  // namespace pmt::parser
