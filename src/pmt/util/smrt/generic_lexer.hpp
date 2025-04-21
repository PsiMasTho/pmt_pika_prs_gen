#pragma once

#include "pmt/base/bitset.hpp"
#include "pmt/util/smrt/state_machine_tables_base.hpp"
#include "pmt/util/smrt/token.hpp"

#include <string_view>

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
  pmt::base::Bitset _accepts_all;
  StateMachineTablesBase const& _tables;
  std::string_view _input;
  size_t _cursor;

 public:
  // -$ Functions $-
  // --$ Lifetime $--
  GenericLexer(std::string_view input_, StateMachineTablesBase const& tables_);

  // --$ Other $--
  auto lex() -> LexReturn;
  auto lex(pmt::base::Bitset::ChunkSpanConst accepts_) -> LexReturn;
};

}  // namespace pmt::util::smrt