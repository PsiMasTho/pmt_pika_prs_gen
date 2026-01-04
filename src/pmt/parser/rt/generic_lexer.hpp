#pragma once

#include "pmt/base/bitset.hpp"
#include "pmt/fw_decl.hpp"
#include "pmt/parser/source_position.hpp"
#include "pmt/parser/token.hpp"
#include "pmt/sm/primitives.hpp"

#include <string_view>

PMT_FW_DECL_NS_CLASS(pmt::parser::rt, LexerTablesBase);

namespace pmt::parser::rt {

class GenericLexer {
 // -$ Types / Constants $-
 static constexpr size_t EXPECTED_REPORTED_MAX = 5;

public:
 class LexReturn {
 public:
  Token _token;
  size_t _accepted;
 };

private:
 // -$ Data $-
 std::vector<pmt::base::Bitset::ChunkType> _accepts_all;
 std::vector<pmt::base::Bitset::ChunkType> _accepts_valid;
 LexerTablesBase const& _lexer_tables;
 std::string_view _input;
 size_t _cursor;
 size_t _linecount_cursor;
 size_t _linecount_last = 0;
 SourcePosition::LinenoType _linecount_at_last = 1;
 pmt::sm::StateNrType _state_nr_linecount;

public:
 // -$ Functions $-
 // --$ Lifetime $--
 GenericLexer(std::string_view input_, LexerTablesBase const& lexer_tables_);

 // --$ Other $--
 auto lex() -> LexReturn;
 auto lex(pmt::base::Bitset::ChunkSpanConst accepts_) -> LexReturn;

 auto get_tables() const -> LexerTablesBase const&;

 [[noreturn]] static void throw_lexing_error(LexerTablesBase const& lexer_tables_, pmt::base::Bitset::ChunkSpanConst accepts_valid_, LexReturn const& lex_return_);

private:
 auto get_source_position_at(size_t p_) -> SourcePosition;
 auto create_lex_return(size_t p_, size_t countl_) -> LexReturn;
 auto is_lex_return_valid(LexReturn const& lex_return_) const -> bool;
};

}  // namespace pmt::parser::rt