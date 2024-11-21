#include "pmt/util/parse/grm_ast_transformations.hpp"

#include "pmt/asserts.hpp"
#include "pmt/util/parse/generic_ast.hpp"
#include "pmt/util/parse/grm_ast.hpp"

#include <cassert>
#include <cmath>
#include <iostream>
#include <limits>
#include <stack>
#include <unordered_map>
#include <unordered_set>
#include <variant>

namespace pmt::util::parse {
namespace {
using RepetitionNumber = std::variant<std::monostate, size_t>;
using RepetitionRange = std::pair<RepetitionNumber, RepetitionNumber>;

auto split_number(std::string_view str_) -> std::pair<std::string_view, std::string_view> {
  size_t const pos = str_.find('#');
  if (pos == std::string_view::npos) {
    return {str_, {}};
  }

  return {str_.substr(0, pos), str_.substr(pos + 1)};
}

auto number_convert(std::string_view str_, size_t base_) -> size_t {
  // clang-format off
  static std::unordered_map<char, size_t> const CHAR_TO_NUM{
   {'0',  0}, {'1',  1}, {'2',  2}, {'3',  3}, {'4',  4}, {'5',  5},
   {'6',  6}, {'7',  7}, {'8',  8}, {'9',  9}, {'a', 10}, {'b', 11},
   {'c', 12}, {'d', 13}, {'e', 14}, {'f', 15}, {'g', 16}, {'h', 17},
   {'i', 18}, {'j', 19}, {'k', 20}, {'l', 21}, {'m', 22}, {'n', 23},
   {'o', 24}, {'p', 25}, {'q', 26}, {'r', 27}, {'s', 28}, {'t', 29},
   {'u', 30}, {'v', 31}, {'w', 32}, {'x', 33}, {'y', 34}, {'z', 35}};
  // clang-format on

  if (base_ < 2 || base_ > CHAR_TO_NUM.size()) {
    throw std::runtime_error("Invalid base: " + std::to_string(base_));
  }

  // Skip leading zeros
  while (!str_.empty() && str_.starts_with('0')) {
    str_.remove_prefix(1);
  }

  size_t ret = 0;
  for (size_t i = str_.size(); i-- > 0;) {
    auto const itr = CHAR_TO_NUM.find(str_[i]);
    if (itr == CHAR_TO_NUM.end() || itr->second >= base_) {
      throw std::runtime_error("Invalid number: " + std::string(str_));
    }

    size_t const step = itr->second * std::pow(base_, str_.size() - i - 1);

    // Check for overflow
    if (std::numeric_limits<size_t>::max() - step < ret) {
      throw std::runtime_error("Number too large: " + std::string(str_));
    }

    ret += step;
  }

  return ret;
}

auto single_char_as_value(GenericAst const& ast_) -> size_t {
  if (ast_.get_id() == GrmAst::TkStringLiteral) {
    std::string const& token = ast_.get_token();
    assert(token.size() == 1);
    return token[0];
  }

  if (ast_.get_id() == GrmAst::TkIntegerLiteral) {
    auto const [base_str, number_str] = split_number(ast_.get_token());
    size_t const base = number_convert(base_str, 10);
    size_t const number = number_convert(number_str, base);
    return number;
  }

  throw std::runtime_error("Invalid single character value");
}

auto get_repetition_number(GenericAst const& token_) -> RepetitionNumber {
  if (token_.get_id() == GrmAst::TkComma) {
    return std::monostate{};
  }

  assert(token_.get_id() == GrmAst::TkIntegerLiteral);
  auto const [base_str, number_str] = split_number(token_.get_token());
  size_t const base = number_convert(base_str, 10);
  size_t const number = number_convert(number_str, base);

  return number;
}

auto get_repetition_range(GenericAst const& repetition_) -> RepetitionRange {
  assert(repetition_.get_id() == GrmAst::NtRepetitionRange);

  switch (repetition_.get_children_size()) {
    case 1: {
      RepetitionNumber const mid = get_repetition_number(*repetition_.get_child_at(0));
      return {mid, mid};
    }
    case 2: {
      RepetitionNumber const lhs = get_repetition_number(*repetition_.get_child_at(0));
      RepetitionNumber const rhs = get_repetition_number(*repetition_.get_child_at(1));
      return {lhs, rhs};
    }
    case 3: {
      RepetitionNumber const lhs = get_repetition_number(*repetition_.get_child_at(0));
      RepetitionNumber const rhs = get_repetition_number(*repetition_.get_child_at(2));
      return {lhs, rhs};
    }
    default:
      throw std::runtime_error("Invalid repetition range");
  }
}

}  // namespace

GrmAstTransformations::GrmAstTransformations(GenericAst& ast_)
 : _ast(ast_) {
}

void GrmAstTransformations::transform() {
  expand_repetitions();
  flatten();
  check_ordering();
}

void GrmAstTransformations::emit_grammar(std::ostream& os_, GenericAst const& ast_) {
  assert(ast_.get_id() == GrmAst::NtGrammar);

  for (size_t i = 0; i < ast_.get_children_size(); ++i) {
    GenericAst const* const production = ast_.get_child_at(i);

    if (production->get_id() != GrmAst::NtTerminalProduction) {
      continue;
    }

    // <text, <parent, idx>, depth>
    using StackItem = std::tuple<std::string, AstPositionConst, size_t>;
    std::stack<StackItem> stack;

    auto const push = [&](StackItem stack_item_) {
      stack.push(std::move(stack_item_));
    };

    auto const take = [&]() -> StackItem {
      auto const node = stack.top();
      stack.pop();
      return node;
    };

    os_ << production->get_child_at(0)->get_token() << " = ";

    push({"", {production, 1}, 0});

    while (!stack.empty()) {
      auto const [text, position, depth] = take();
      auto const [parent, idx] = position;
      auto const child = (parent == nullptr) ? nullptr : parent->get_child_at(idx);

      os_ << text;

      if (child == nullptr) {
        continue;
      }

      switch (child->get_id()) {
        case GrmAst::NtChoices: {
          size_t const size = child->get_children_size();

          if (size > 1 && depth > 0) {
            push({")", {nullptr, 0}, 0});
          }

          for (size_t i = size; i--;) {
            std::string const delim = (i == 0) ? "" : " | ";
            size_t const depth_incr = (size > 1) ? 1 : 0;
            push({delim, {child, i}, depth + depth_incr});
          }

          if (size > 1 && depth > 0) {
            push({"(", {nullptr, 0}, 0});
          }
        } break;
        case GrmAst::NtSequence: {
          size_t const size = child->get_children_size();
          for (size_t i = size; i--;) {
            std::string const delim = (i == 0) ? "" : " ";
            size_t const depth_incr = (size > 1) ? 1 : 0;
            push({delim, {child, i}, depth + depth_incr});
          }
        } break;
        case GrmAst::NtRepetition:
          push({"", {child, 1}, depth + 1});
          push({"", {child, 0}, depth + 1});
          break;
        case GrmAst::NtRepetitionRange:
          os_ << "{";
          for (size_t i = 0; i < child->get_children_size(); ++i) {
            os_ << child->get_child_at(i)->get_token();
          }
          os_ << "}";
          break;
        case GrmAst::TkTerminalIdentifier:
          os_ << child->get_token();
          break;
        case GrmAst::NtRange:
          os_ << "[10#" << single_char_as_value(*child->get_child_at(0)) << "..10#" << single_char_as_value(*child->get_child_at(1)) << "]";
          break;
        case GrmAst::TkEpsilon:
          os_ << "epsilon";
          break;
        case GrmAst::TkStringLiteral:
          os_ << R"(")" << position.first->get_child_at(position.second)->get_token() << R"(")";
          break;
        case GrmAst::TkIntegerLiteral:
          os_ << child->get_token();
          break;
      }
    }

    os_ << ";\n\n";
  }
}

void GrmAstTransformations::expand_repetitions() {
  for (size_t i = 0; i < _ast.get_children_size(); ++i) {
    GenericAst* const production = _ast.get_child_at(i);

    if (production->get_id() != GrmAst::NtTerminalProduction) {
      continue;
    }

    std::stack<AstPosition> stack;
    stack.emplace(production, 1);

    while (!stack.empty()) {
      auto [parent, idx] = stack.top();
      stack.pop();
      GenericAst* const child = parent->get_child_at(idx);
      switch (child->get_id()) {
        case GrmAst::NtChoices:
        case GrmAst::NtSequence:
          for (size_t j = 0; j < child->get_children_size(); ++j) {
            stack.emplace(child, j);
          }
          break;
        case GrmAst::NtRepetition: {
          size_t const size_before = _new_productions.size();
          expand_repetition({parent, idx});
          size_t const size_after = _new_productions.size();
          stack.emplace(parent, idx);
          for (size_t j = size_before; j < size_after; ++j) {
            stack.emplace(_new_productions[j].get(), 1);
          }
          break;
        }
        default:
          break;
      }
    }

    // Empty the new production queue above the current production
    size_t const incr = _new_productions.size();
    while (!_new_productions.empty()) {
      _ast.give_child_at(i, std::move(_new_productions.front()));
      _new_productions.pop_front();
    }
    i += incr;
  }
}

void GrmAstTransformations::flatten() {
  for (size_t i = 0; i < _ast.get_children_size(); ++i) {
    GenericAst* const production = _ast.get_child_at(i);

    if (production->get_id() != GrmAst::NtTerminalProduction) {
      continue;
    }

    bool repeat = false;

    do {
      repeat = false;
      std::stack<AstPosition> stack;
      stack.emplace(production, 1);

      while (!stack.empty()) {
        auto [parent, idx] = stack.top();
        stack.pop();
        GenericAst* const child = parent->get_child_at(idx);

        switch (child->get_id()) {
          case GrmAst::NtChoices:
            if (child->get_children_size() == 1) {
              GenericAst::UniqueHandle grandchild = child->take_child_at(0);
              parent->take_child_at(idx);
              parent->give_child_at(idx, std::move(grandchild));
              stack.emplace(parent, idx);
              repeat = true;
            } else {
              // Any choices in choices can be immediately flattened
              for (size_t j = 0; j < child->get_children_size(); ++j) {
                if (child->get_child_at(j)->get_id() != GrmAst::NtChoices) {
                  continue;
                }

                GenericAst::UniqueHandle grandchild = child->take_child_at(j);
                while (grandchild->get_children_size() > 0) {
                  child->give_child_at(child->get_children_size(), grandchild->take_child_at(grandchild->get_children_size() - 1));
                }
                j = 0;
                repeat = true;
              }

              for (size_t j = 0; j < child->get_children_size(); ++j) {
                stack.emplace(child, j);
              }
            }
            break;
          case GrmAst::NtSequence:
            // Unpack any nested sequences
            for (size_t j = 0; j < child->get_children_size(); ++j) {
              GenericAst* const grandchild = child->get_child_at(j);
              if (grandchild->get_id() != GrmAst::NtSequence) {
                continue;
              }
              child->unpack(j);
              repeat = true;
              j = 0;
            }
            // Merge neighboring string literals
            for (size_t j = 0; j < child->get_children_size() - 1; ++j) {
              GenericAst* const lhs = child->get_child_at(j);
              GenericAst* const rhs = child->get_child_at(j + 1);
              if (lhs->get_id() != GrmAst::TkStringLiteral || rhs->get_id() != GrmAst::TkStringLiteral) {
                continue;
              }
              lhs->set_token(lhs->get_token() + rhs->get_token());
              child->take_child_at(j + 1);
              repeat = true;
              j = 0;
            }
            // Merge neighboring epsilon tokens
            for (size_t j = 0; j < child->get_children_size() - 1; ++j) {
              GenericAst* const lhs = child->get_child_at(j);
              GenericAst* const rhs = child->get_child_at(j + 1);
              if (lhs->get_id() != GrmAst::TkEpsilon || rhs->get_id() != GrmAst::TkEpsilon) {
                continue;
              }
              child->take_child_at(j + 1);
              repeat = true;
              j = 0;
            }
            if (child->get_children_size() == 1) {
              GenericAst::UniqueHandle grandchild = child->take_child_at(0);
              parent->take_child_at(idx);
              parent->give_child_at(idx, std::move(grandchild));
              stack.emplace(parent, idx);
              repeat = true;
            } else {
              bool hasChoices = false;
              for (size_t j = 0; j < child->get_children_size(); ++j) {
                if (child->get_child_at(j)->get_id() != GrmAst::NtChoices) {
                  continue;
                }
                hasChoices = true;

                GenericAst::UniqueHandle grandchild = child->take_child_at(j);

                GenericAst::UniqueHandle new_choice = GenericAst::construct(GenericAst::Tag::Children);
                new_choice->set_id(GrmAst::NtChoices);

                for (size_t k = 0; k < grandchild->get_children_size(); ++k) {
                  GenericAst::UniqueHandle cloned = GenericAst::clone(*child);
                  cloned->give_child_at(j, GenericAst::clone(*grandchild->get_child_at(k)));
                  new_choice->give_child_at(k, std::move(cloned));
                }

                parent->take_child_at(idx);
                parent->give_child_at(idx, std::move(new_choice));
                stack.emplace(parent, idx);
                repeat = true;
                break;
              }
              if (!hasChoices) {
                for (size_t j = 0; j < child->get_children_size(); ++j) {
                  stack.emplace(child, j);
                }
              }
            }
            break;
          case GrmAst::NtRepetition:
            stack.emplace(child, 0);
            break;
          default:
            break;
        }
      }
    } while (repeat);
  }
}

void GrmAstTransformations::check_ordering() {
  std::unordered_set<std::string_view> defined;
  for (size_t i = 0; i < _ast.get_children_size(); ++i) {
    GenericAst* const production = _ast.get_child_at(i);
    if (production->get_id() != GrmAst::NtTerminalProduction) {
      continue;
    }

    std::string_view const terminalName = production->get_child_at(0)->get_token();

    // We can only use terminals that have been defined earlier
    std::stack<GenericAst const*> stack;

    auto const take = [&]() -> GenericAst const* {
      auto const node = stack.top();
      stack.pop();
      return node;
    };

    stack.push(production->get_child_at(1));

    while (!stack.empty()) {
      auto const cur = take();

      switch (cur->get_id()) {
        case GrmAst::NtRepetition:
          stack.push(cur->get_child_at(1));
          break;
        case GrmAst::NtChoices:
        case GrmAst::NtSequence:
          for (size_t j = 0; j < cur->get_children_size(); ++j) {
            stack.push(cur->get_child_at(j));
          }
          break;
        case GrmAst::TkTerminalIdentifier:
          if (terminalName != cur->get_token() && defined.find(cur->get_token()) == defined.end()) {
            throw std::runtime_error("Terminal '" + std::string(terminalName) + "' uses undefined terminal '" + cur->get_token() + "'");
          }
          break;
        default:
          break;
      }
    }

    defined.insert(terminalName);
  }
}

auto GrmAstTransformations::make_anonymous_zero_or_more(AstPosition ast_position_) -> size_t {
  std::string const anon_name = get_next_anonymous_definition_name();
  GenericAst::UniqueHandle anon_production = GenericAst::construct(GenericAst::Tag::Children);
  anon_production->set_id(GrmAst::NtTerminalProduction);
  GenericAst::UniqueHandle anon_name_token = GenericAst::construct(GenericAst::Tag::Token);
  anon_name_token->set_id(GrmAst::TkTerminalIdentifier);
  anon_name_token->set_token(anon_name);
  anon_production->give_child_at(0, std::move(anon_name_token));
  GenericAst::UniqueHandle anon_choices = GenericAst::construct(GenericAst::Tag::Children);
  anon_choices->set_id(GrmAst::NtChoices);
  GenericAst::UniqueHandle anon_epsilon = GenericAst::construct(GenericAst::Tag::Token);
  anon_epsilon->set_id(GrmAst::TkEpsilon);
  anon_choices->give_child_at(0, std::move(anon_epsilon));
  GenericAst::UniqueHandle anon_sequence = GenericAst::construct(GenericAst::Tag::Children);
  anon_sequence->set_id(GrmAst::NtSequence);
  anon_sequence->give_child_at(0, GenericAst::clone(*ast_position_.first->get_child_at(ast_position_.second)->get_child_at(0)));
  GenericAst::UniqueHandle anon_reference = GenericAst::construct(GenericAst::Tag::Token);
  anon_reference->set_id(GrmAst::TkTerminalIdentifier);
  anon_reference->set_token(anon_name);
  anon_sequence->give_child_at(1, std::move(anon_reference));
  anon_choices->give_child_at(1, std::move(anon_sequence));
  anon_production->give_child_at(1, std::move(anon_choices));
  _new_productions.emplace_back(std::move(anon_production));
  return _new_productions.size() - 1;
}

auto GrmAstTransformations::get_next_anonymous_definition_name() -> std::string {
  std::string ret = std::to_string(_anonymous_definition_counter++);
  ret = GrmAstTransformations::ANONYMOUS_DEFINITION_PREFIX + std::string(GrmAstTransformations::ANONYMOUS_DEFINITION_DIGIT_COUNT - ret.size(), '0') + ret;
  return ret;
}

void GrmAstTransformations::expand_repetition(AstPosition ast_position_) {
  assert(ast_position_.first->get_child_at(ast_position_.second)->get_id() == GrmAst::NtRepetition);

  GenericAst::UniqueHandle replacement = GenericAst::construct(GenericAst::Tag::Children);
  replacement->set_id(GrmAst::NtChoices);

  RepetitionRange const range = get_repetition_range(*ast_position_.first->get_child_at(ast_position_.second)->get_child_at(1));

  bool const is_bounded[2] = {std::holds_alternative<size_t>(range.first), std::holds_alternative<size_t>(range.second)};

  if (is_bounded[0] && is_bounded[1]) {
    replacement->give_child_at(0, make_exact_repetition_range_choices(ast_position_, std::get<size_t>(range.first), std::get<size_t>(range.second)));
  } else if (is_bounded[0] && !is_bounded[1]) {
    GenericAst::UniqueHandle sequence = make_exact_repetition_sequence(ast_position_, std::get<size_t>(range.first));
    size_t const new_production_idx = make_anonymous_zero_or_more(ast_position_);
    GenericAst::UniqueHandle anon_reference = GenericAst::construct(GenericAst::Tag::Token);
    anon_reference->set_id(GrmAst::TkTerminalIdentifier);
    anon_reference->set_token(_new_productions[new_production_idx]->get_child_at(0)->get_token());
    sequence->give_child_at(sequence->get_children_size(), std::move(anon_reference));
    replacement->give_child_at(replacement->get_children_size(), std::move(sequence));
  } else if (!is_bounded[0] && is_bounded[1]) {
    replacement->give_child_at(0, make_exact_repetition_range_choices(ast_position_, 0, std::get<size_t>(range.second)));
  } else {
    size_t const new_production_idx = make_anonymous_zero_or_more(ast_position_);
    GenericAst::UniqueHandle anon_reference = GenericAst::construct(GenericAst::Tag::Token);
    anon_reference->set_id(GrmAst::TkTerminalIdentifier);
    anon_reference->set_token(_new_productions[new_production_idx]->get_child_at(0)->get_token());
    replacement->give_child_at(0, std::move(anon_reference));
  }

  ast_position_.first->take_child_at(ast_position_.second);
  ast_position_.first->give_child_at(ast_position_.second, std::move(replacement));
}

auto GrmAstTransformations::make_exact_repetition_sequence(AstPosition ast_position_, size_t count_) -> GenericAst::UniqueHandle {
  GenericAst::UniqueHandle sequence = GenericAst::construct(GenericAst::Tag::Children);
  sequence->set_id(GrmAst::NtSequence);
  for (size_t i = 0; i < count_; ++i) {
    sequence->give_child_at(i, GenericAst::clone(*ast_position_.first->get_child_at(ast_position_.second)->get_child_at(0)));
  }
  return sequence;
}

auto GrmAstTransformations::make_exact_repetition_range_choices(AstPosition ast_position_, size_t min_count_, size_t max_count_) -> GenericAst::UniqueHandle {
  GenericAst::UniqueHandle choices = GenericAst::construct(GenericAst::Tag::Children);
  choices->set_id(GrmAst::NtChoices);
  if (min_count_ == 0) {
    GenericAst::UniqueHandle epsilon = GenericAst::construct(GenericAst::Tag::Token);
    epsilon->set_id(GrmAst::TkEpsilon);
    epsilon->set_token("epsilon");
    choices->give_child_at(0, std::move(epsilon));
    ++min_count_;
  }
  for (size_t i = min_count_; i <= max_count_; ++i) {
    choices->give_child_at(i - min_count_, make_exact_repetition_sequence(ast_position_, i));
  }
  return choices;
}

}  // namespace pmt::util::parse