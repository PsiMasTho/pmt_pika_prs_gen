#pragma once

#include "pmt/fw_decl.hpp"
#include "pmt/base/bitset.hpp"
#include "pmt/util/smrt/token.hpp"
#include "pmt/util/smrt/state_machine_primitives.hpp"
#include "pmt/util/smrt/source_position.hpp"

#include <string_view>

PMT_FW_DECL_NS_CLASS(pmt::util::smrt, LexerTablesBase);

namespace pmt::util::smrt {

class GenericLexer {
  public:
  // -$ Types / Constants $-
  class LexReturn {
   public:
    Token _token;
    size_t _accepted;
  };

 private:
  // -$ Data $-
  size_t _accept_count;
  std::vector<pmt::base::Bitset::ChunkType> _accepts_all;
  std::vector<pmt::base::Bitset::ChunkType> _accepts_valid;
  LexerTablesBase const& _lexer_tables;
  std::string_view _input;
  size_t _cursor;
  size_t _linecount_cursor;
  size_t _linecount_last = 0;
  SourcePosition::LinenoType _linecount_at_last = 1;
  pmt::util::smrt::StateNrType _state_nr_linecount;

 public:
  // -$ Functions $-
  // --$ Lifetime $--
  GenericLexer(std::string_view input_, LexerTablesBase const& lexer_tables_);

  // --$ Other $--
  auto lex() -> LexReturn;
  auto lex(pmt::base::Bitset::ChunkSpanConst accepts_) -> LexReturn;

  auto get_eoi_accept_index() const -> size_t;

 private:
  auto get_source_position_at(size_t p_) -> SourcePosition;
};

}  // namespace pmt::util::smrt