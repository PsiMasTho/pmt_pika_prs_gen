#pragma once

#include "pmt/util/parse/generic_ast.hpp"

#include <deque>
#include <string>

namespace pmt::util::parse {

class GrmAstTransformations {
 public:
  static inline size_t const ANONYMOUS_DEFINITION_DIGIT_COUNT = 4;
  static inline char const *const ANONYMOUS_DEFINITION_PREFIX = "$__anon_";

  explicit GrmAstTransformations(GenericAst &ast_);

  void transform();

  static void emit_grammar(std::ostream &os_, GenericAst const &ast_);

 private:
  // - Types / Private Constants -
  using AstPosition = std::pair<GenericAst *, size_t>;
  using AstPositionConst = std::pair<GenericAst const *, size_t>;

  // - Transformations -
  void expand_repetitions();
  void flatten();
  void check_ordering();

  // - Helpers -
  auto make_anonymous_zero_or_more(AstPosition ast_position_) -> size_t;
  auto get_next_anonymous_definition_name() -> std::string;
  void expand_repetition(AstPosition ast_position_);
  static auto make_exact_repetition_sequence(AstPosition ast_position_, size_t count_) -> GenericAst::UniqueHandle;
  static auto make_exact_repetition_range_choices(AstPosition ast_position_, size_t min_count_, size_t max_count_) -> GenericAst::UniqueHandle;

  // - Data -
  GenericAst &_ast;
  size_t _anonymous_definition_counter = 0;
  std::deque<GenericAst::UniqueHandle> _new_productions;
};

}  // namespace pmt::util::parse