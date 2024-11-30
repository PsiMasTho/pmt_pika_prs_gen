#pragma once

#include "pmt/util/parse/ast_position.hpp"
#include "pmt/util/parse/generic_ast.hpp"

#include <deque>
#include <optional>
#include <string>

namespace pmt::util::parse {

class GrmAstTransformations {
 public:
  using NumberType = std::uint64_t;
  using RepetitionNumberType = std::optional<NumberType>;
  using RepetitionRangeType = std::pair<RepetitionNumberType, RepetitionNumberType>;

  static inline size_t const ANONYMOUS_DEFINITION_DIGIT_COUNT = 4;
  static inline char const *const ANONYMOUS_DEFINITION_PREFIX = "$__anon_";

  explicit GrmAstTransformations(GenericAst &ast_);

  void transform();
  static void emit_grammar(std::ostream &os_, GenericAst const &ast_);

  static auto single_char_as_value(GenericAst const &ast_) -> NumberType;
  static auto split_number(std::string_view str_) -> std::pair<std::string_view, std::string_view>;
  static auto number_convert(std::string_view str_, NumberType base_) -> NumberType;
  static auto get_repetition_number(GenericAst const &token_) -> RepetitionNumberType;
  static auto get_repetition_range(GenericAst const &repetition_) -> RepetitionRangeType;

 private:
  // - Transformations -
  void expand_repetitions();
  void flatten();
  void check_ordering();

  // - Helpers -
  void expand_repetition(AstPosition ast_position_);
  auto get_next_anonymous_definition_name() -> std::string;
  auto make_epsilon() -> GenericAst::UniqueHandle;
  auto make_exact_repetition(GenericAst::UniqueHandle item_, size_t count_) -> GenericAst::UniqueHandle;
  auto make_repetition_range(GenericAst::UniqueHandle item_, RepetitionNumberType min_count_, RepetitionNumberType max_count_) -> GenericAst::UniqueHandle;
  auto make_anonymous_definition(GenericAst::UniqueHandle production_) -> GenericAst::UniqueHandle;
  // - Data -
  GenericAst &_ast;
  size_t _anonymous_definition_counter = 0;
  std::deque<GenericAst::UniqueHandle> _new_productions;
};

}  // namespace pmt::util::parse