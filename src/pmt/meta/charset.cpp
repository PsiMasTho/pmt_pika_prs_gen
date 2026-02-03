#include "pmt/meta/charset.hpp"

#include "pmt/ast/ast.hpp"
#include "pmt/meta/ids.hpp"
#include "pmt/meta/number.hpp"

namespace pmt::meta {
using namespace pmt::container;
using namespace pmt::rt;

#include <cassert>

namespace {
using IntervalType = Interval<Charset::SetType::KeyType>;

auto get_interval(pmt::ast::Ast const &ast_) -> IntervalType {
 if (ast_.get_id() == Ids::CharsetRange) {
  return IntervalType(Number(*ast_.get_child_at(0)).get_value(), Number(*ast_.get_child_at(1)).get_value());
 }

 return IntervalType(Number(ast_).get_value());
}

auto get_set(pmt::ast::Ast const &ast_) -> Charset::SetType {
 assert(ast_.get_id() == Ids::Charset);
 Charset::SetType ret;
 for (size_t i = 0; i < ast_.get_children_size(); ++i) {
  ret.insert(get_interval(*ast_.get_child_at(i)));
 }

 return ret;
}
}  // namespace

Charset::Charset(pmt::ast::Ast const &ast_)
 : _values(get_set(ast_)) {
}

auto Charset::get_values() const -> SetType const & {
 return _values;
}

}  // namespace pmt::meta