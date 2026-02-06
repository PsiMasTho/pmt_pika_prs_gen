#include "pmt/meta/charset_literal.hpp"

#include "pmt/hash.hpp"
#include "pmt/meta/ids.hpp"
#include "pmt/meta/number.hpp"
#include "pmt/rt/ast.hpp"
#include "pmt/unreachable.hpp"

#include <cassert>
#include <limits>

namespace pmt::meta {
using namespace pmt::container;
using namespace pmt::rt;

namespace {
auto interval_from_ast(pmt::rt::Ast const& ast_) -> CharsetLiteral::IntervalType {
 if (ast_.get_id() == Ids::CharsetRange) {
  return CharsetLiteral::IntervalType(Number(*ast_.get_child_at(0)).get_value(), Number(*ast_.get_child_at(1)).get_value());
 }

 return CharsetLiteral::IntervalType(Number(ast_, std::numeric_limits<SymbolType>::max()).get_value());
}

auto charset_from_ast(pmt::rt::Ast const& ast_) -> CharsetLiteral::SetType {
 assert(ast_.get_id() == Ids::Charset);
 CharsetLiteral::SetType ret;
 for (size_t i = 0; i < ast_.get_children_size(); ++i) {
  ret.insert(interval_from_ast(*ast_.get_child_at(i)));
 }

 return ret;
}

}  // namespace

CharsetLiteral::CharsetLiteral(pmt::rt::Ast const& ast_) {
 switch (ast_.get_id()) {
  case Ids::Charset:
   push_back(charset_from_ast(ast_));
   break;
  case Ids::CharacterLiteral: {
   push_back(ast_.get_string().front());
  } break;
  case Ids::IntegerLiteral: {
   push_back(Number(ast_).get_value());
  } break;
  case Ids::StringLiteral: {
   for (char const c : ast_.get_string()) {
    push_back(c);
   }
  } break;
  default:
   pmt::unreachable();
 }
}

auto CharsetLiteral::get_symbol_set_at(size_t idx_) const -> SetType const& {
 assert(idx_ < size());
 return _literal[idx_];
}

auto CharsetLiteral::get_symbol_set_at(size_t idx_) -> SetType& {
 assert(idx_ < size());
 return _literal[idx_];
}

void CharsetLiteral::push_back(SetType symbol_set_) {
 _literal.emplace_back();
 get_symbol_set_at(size() - 1) = std::move(symbol_set_);
}

void CharsetLiteral::push_back(IntervalType interval_) {
 push_back(SetType(interval_));
}

void CharsetLiteral::push_back(SymbolType symbol_) {
 push_back(Interval<SymbolType>(symbol_));
}

void CharsetLiteral::pop_back() {
 _literal.pop_back();
}

auto CharsetLiteral::size() const -> size_t {
 return _literal.size();
}

}  // namespace pmt::meta

namespace std {
auto hash<pmt::meta::CharsetLiteral>::operator()(pmt::meta::CharsetLiteral const& literal_) const -> size_t {
 size_t seed = pmt::Hash::Phi64;

 for (size_t i = 0; i < literal_.size(); ++i) {
  pmt::Hash::combine(literal_.get_symbol_set_at(i), seed);
  pmt::Hash::combine(literal_.get_symbol_set_at(i), seed);
 }

 return seed;
}
}  // namespace std