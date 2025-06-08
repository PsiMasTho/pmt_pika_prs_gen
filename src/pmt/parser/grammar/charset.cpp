#include "pmt/parser/grammar/charset.hpp"

#include "pmt/parser/grammar/ast.hpp"
#include "pmt/parser/generic_ast.hpp"
#include "pmt/parser/grammar/number.hpp"

namespace pmt::parser::grammar {
using namespace pmt::base;
using namespace pmt::util::sm;

#include <cassert>

namespace {
using IntervalType = Interval<Charset::SetType::KeyType>;

auto get_interval(pmt::parser::GenericAst const &ast_) -> IntervalType {
 if (ast_.get_id() == Ast::NtCharsetRange) {
  return IntervalType(Number(*ast_.get_child_at(0)).get_value(), Number(*ast_.get_child_at(1)).get_value()
  );
 }

 return IntervalType(Number(ast_).get_value());
}

auto get_set(pmt::parser::GenericAst const &ast_) -> Charset::SetType {
 assert(ast_.get_id() == Ast::NtTerminalCharset);
 Charset::SetType ret;
 for (size_t i = 0; i < ast_.get_children_size(); ++i) {
  ret.insert(get_interval(*ast_.get_child_at(i)));
 }

 return ret;
}
}

Charset::Charset(pmt::parser::GenericAst const &ast_)
: _values(get_set(ast_)) {
}

auto Charset::get_values() const -> SetType const& {
 return _values;
}

}