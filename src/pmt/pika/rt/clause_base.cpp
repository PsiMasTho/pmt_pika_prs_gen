#include "pmt/pika/rt/clause_base.hpp"

#include <cassert>
#include <exception>

namespace pmt::pika::rt {

auto ClauseBase::has_literal_id() const -> bool {
 return get_tag() == Tag::CharsetLiteral;
}

auto ClauseBase::has_rule_id() const -> bool {
 return get_tag() == Tag::Identifier;
}

auto ClauseBase::tag_to_string(Tag tag_) -> std::string {
 switch (tag_) {
  case Tag::Sequence:
   return "Sequence";
  case Tag::Choice:
   return "Choice";
  case Tag::Identifier:
   return "Identifier";
  case Tag::CharsetLiteral:
   return "CharsetLiteral";
  case Tag::NegativeLookahead:
   return "NegativeLookahead";
  case Tag::Epsilon:
   return "Epsilon";
 }

 std::terminate();  // unreachable
}

}  // namespace pmt::pika::rt
