#pragma once

#include "pmt/util/parse/generic_ast.hpp"

#include <deque>
#include <optional>
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

  using RepetitionNumber = std::optional<size_t>;
  using RepetitionRange = std::pair<RepetitionNumber, RepetitionNumber>;

  // - Transformations -
  void expand_repetitions();
  void flatten();
  void check_ordering();

  // - Helpers -
  auto get_repetition_number(GenericAst const &token_) -> RepetitionNumber;
  auto get_repetition_range(GenericAst const &repetition_) -> RepetitionRange;
  void expand_repetition(AstPosition ast_position_);
  auto get_next_anonymous_definition_name() -> std::string;
  auto make_epsilon() -> GenericAst::UniqueHandle;
  auto make_exact_repetition(GenericAst::UniqueHandle item_, size_t count_) -> GenericAst::UniqueHandle;
  auto make_repetition_range(GenericAst::UniqueHandle item_, RepetitionNumber min_count_, RepetitionNumber max_count_) -> GenericAst::UniqueHandle;
  auto make_anonymous_definition(GenericAst::UniqueHandle production_) -> GenericAst::UniqueHandle;

  // - Data -
  GenericAst &_ast;
  size_t _anonymous_definition_counter = 0;
  std::deque<GenericAst::UniqueHandle> _new_productions;
};

}  // namespace pmt::util::parse