#include "pmt/parser/grammar/grammar_printer.hpp"

#include "parser/grammar/repetition_range.hpp"
#include "pmt/base/overloaded.hpp"
#include "pmt/parser/grammar/grammar.hpp"
#include "pmt/parser/grammar/rule.hpp"
#include "primitives.hpp"

#include <cassert>
#include <set>
#include <utility>

namespace pmt::parser::grammar {
using namespace pmt::base;
using namespace pmt::util::sm;

namespace {
using StackItem = std::variant<std::string, RuleExpression const*>;

std::string const BOOLALPHA[2]{"false", "true"};  // NOLINT

class Locals {
public:
 std::vector<StackItem> _pending;
};

auto take(Locals& locals_) -> StackItem {
 StackItem const node = std::move(locals_._pending.back());
 locals_._pending.pop_back();
 return node;
}

auto push(Locals& locals_, StackItem item_) {
 locals_._pending.push_back(std::move(item_));
}

void write_rule_lhs(GrammarPrinter::Args& args_, Locals& locals_, std::string const& rule_name_, Rule const& rule_) {
 std::string delim;
 std::string parameter_str;

 if (!rule_._parameters._id_string.empty()) {
  parameter_str += std::exchange(delim, ", ") + "id = \"" + rule_._parameters._id_string + "\"";
 }

 if (rule_._parameters._display_name != rule_name_) {
  parameter_str += std::exchange(delim, ", ") + "display_name = \"" + rule_._parameters._display_name + "\"";
 }

 if (rule_._parameters._merge != RuleParameters::MERGE_DEFAULT) {
  parameter_str += std::exchange(delim, ", ") + "merge = " + BOOLALPHA[rule_._parameters._merge];
 }

 if (rule_._parameters._unpack != RuleParameters::UNPACK_DEFAULT) {
  parameter_str += std::exchange(delim, ", ") + "unpack = " + BOOLALPHA[rule_._parameters._unpack];
 }

 if (!parameter_str.empty()) {
  parameter_str = "<" + parameter_str + ">";
 }

 args_._out << rule_name_ << parameter_str << " = ";
}

void write_rule_rhs(GrammarPrinter::Args& args_, Locals& locals_, Rule const& rule_) {
 push(locals_, ";");
 push(locals_, rule_._definition.get());

 while (!locals_._pending.empty()) {
  StackItem const cur = take(locals_);
  std::visit(pmt::base::Overloaded{[&](std::string const& str_) { args_._out << str_; },
                                   [&](RuleExpression const* expr_) {
                                    if (expr_ == nullptr) {
                                     push(locals_, "/* missing expression */");
                                     return;
                                    }
                                    switch (expr_->get_tag()) {
                                     case RuleExpression::Tag::Sequence: {
                                      push(locals_, ")");
                                      std::string delim;
                                      for (size_t i = expr_->get_children_size(); i--;) {
                                       push(locals_, std::exchange(delim, " "));
                                       push(locals_, expr_->get_child_at(i));
                                      }
                                      push(locals_, "(");
                                     } break;
                                     case RuleExpression::Tag::Choice: {
                                      push(locals_, ")");
                                      std::string delim;
                                      for (size_t i = expr_->get_children_size(); i--;) {
                                       push(locals_, std::exchange(delim, " | "));
                                       push(locals_, expr_->get_child_at(i));
                                      }
                                      push(locals_, "(");
                                     } break;
                                     case RuleExpression::Tag::Hidden: {
                                      push(locals_, ")~");
                                      push(locals_, expr_->get_child_at(0));
                                      push(locals_, "(");
                                     } break;
                                     case RuleExpression::Tag::Identifier: {
                                      args_._out << expr_->get_identifier();
                                     } break;
                                     case RuleExpression::Tag::Literal: {
                                      std::string delim;
                                      for (IntervalSet<SymbolValueType> const& sym : expr_->get_literal()) {
                                       args_._out << std::exchange(delim, " ") << "[";
                                       sym.for_each_interval([&](Interval<SymbolValueType> interval_) { args_._out << "10#" << interval_.get_lower() << "..10#" << interval_.get_upper(); });
                                       args_._out << "]";
                                      }
                                     } break;
                                     case RuleExpression::Tag::Repetition: {
                                      std::string rep_string = "){";
                                      RepetitionRange const& rep_range = expr_->get_repetition_range();
                                      rep_string += "10#" + std::to_string(rep_range.get_lower());
                                      rep_string += ",";
                                      if (rep_range.get_upper().has_value()) {
                                       rep_string += "10#" + std::to_string(*rep_range.get_upper());
                                      }
                                      rep_string += "}";
                                      push(locals_, rep_string);
                                      push(locals_, expr_->get_child_at(0));
                                      push(locals_, "(");
                                     } break;
                                    }
                                   }},
             cur);
 }
}

void write_start_rule(GrammarPrinter::Args& args_, Locals& locals_, std::string const& rule_name_) {
 args_._out << "@start = " << (rule_name_.empty() ? "/* missing identifier */" : rule_name_) << ";\n";
}

void write_rule(GrammarPrinter::Args& args_, Locals& locals_, std::string const& rule_name_) {
 Rule const* rule = args_._grammar.get_rule(rule_name_);
 assert(rule != nullptr);
 write_rule_lhs(args_, locals_, rule_name_, *rule);
 write_rule_rhs(args_, locals_, *rule);
}

}  // namespace

void GrammarPrinter::print(Args args_) {
 Locals locals;

 std::set<std::string> const _rule_names = [&] {
  auto unsorted = args_._grammar.get_rule_names();
  return std::set<std::string>(unsorted.begin(), unsorted.end());
 }();

 write_start_rule(args_, locals, args_._grammar.get_start_rule_name());

 std::string delim;
 for (auto const& rule_name : _rule_names) {
  args_._out << std::exchange(delim, "\n");
  write_rule(args_, locals, rule_name);
 }
}
}  // namespace pmt::parser::grammar