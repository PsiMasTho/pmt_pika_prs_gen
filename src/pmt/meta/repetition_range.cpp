#include "pmt/meta/repetition_range.hpp"

#include "pmt/ast/ast.hpp"
#include "pmt/meta/ids.hpp"
#include "pmt/unreachable.hpp"

#include <cassert>

namespace pmt::meta {
using namespace pmt::ast;

namespace {
auto get_repetition_number(Ast const& token_) -> std::optional<Number::NumberType> {
 if (token_.get_id() != Ids::IntegerLiteral) {
  return std::nullopt;
 }

 return Number(token_).get_value();
}

}  // namespace

RepetitionRange::RepetitionRange(pmt::ast::Ast const& ast_) {
 switch (ast_.get_id()) {
  case Ids::Plus: {
   _lower = 1;
  } break;
  case Ids::Star: {
   _lower = 0;
   _upper = std::nullopt;
  } break;
  case Ids::Question: {
   _lower = 0;
   _upper = 1;
  } break;
  case Ids::RepetitionRange: {
   switch (ast_.get_children_size()) {
    case 1: {
     std::optional<Number::NumberType> num = get_repetition_number(*ast_.get_child_at(0));
     _lower = num.value_or(0);
     _upper = num;
    } break;
    case 2: {
     _lower = get_repetition_number(*ast_.get_child_at(0)).value_or(0);
     _upper = get_repetition_number(*ast_.get_child_at(1));
    } break;
    case 3: {
     _lower = get_repetition_number(*ast_.get_child_at(0)).value_or(0);
     _upper = get_repetition_number(*ast_.get_child_at(2));
    } break;
    default:
     throw std::runtime_error("Invalid repetition range: wrong number of children");
   }
  } break;
  default: {
   pmt::unreachable();
  }
 }

 if (_upper.has_value() && _upper.value() < _lower) {
  throw std::runtime_error("Invalid repetition range: upper bound is less than lower bound");
 }
}

auto RepetitionRange::get_lower() const -> Number::NumberType {
 return _lower;
}

auto RepetitionRange::get_upper() const -> std::optional<Number::NumberType> {
 return _upper;
}

void RepetitionRange::set_lower(Number::NumberType lower_) {
 _lower = lower_;
 if (_upper.has_value() && *_upper < lower_) {
  _upper = lower_;
 }
}

void RepetitionRange::set_upper(std::optional<Number::NumberType> upper_) {
 _upper = upper_;
 if (_upper.has_value() && _lower > *_upper) {
  _lower = _upper.value();
 }
}

}  // namespace pmt::meta