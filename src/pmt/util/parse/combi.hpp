#pragma once

#include "pmt/base/string_literal.hpp"
#include "pmt/util/parse/generic_ast.hpp"

namespace pmt::util::parse {

template <typename CHAR_TYPE_>
class combi{
public:
 using generic_ast = generic_ast<CHAR_TYPE_>;

 class context {
  public:
  CHAR_TYPE_ const* _begin;
  CHAR_TYPE_ const* _end;
  CHAR_TYPE_ const* _cursor;
 };

//-- Combinators ---------------------------------------------------------------
 template <generic_ast_base::id_type ID_, typename... TS_>
 class seq{
  public:
   static auto exec(context& ctx_) -> generic_ast*;
 };

 template <generic_ast_base::id_type ID_, typename... TS_>
 class sor {
  public:
   static auto exec(context& ctx_) -> generic_ast*;
 };

 template <generic_ast_base::id_type ID_, typename T_>
 class plus {
  public:
   static auto exec(context& ctx_) -> generic_ast*;
 };

 template <generic_ast_base::id_type ID_, typename T_>
 class star {
  public:
   static auto exec(context& ctx_) -> generic_ast*;
 };

 template <generic_ast_base::id_type ID_, typename T_>
 class opt {
  public:
   static auto exec(context& ctx_) -> generic_ast*;
 };

 //-- Character parsers --------------------------------------------------------
 template <generic_ast_base::id_type ID_, CHAR_TYPE_ C_>
 class ch {
  public:
   static auto exec(context& ctx_) -> generic_ast*;
 };

 template <generic_ast_base::id_type ID_, CHAR_TYPE_ MIN_, CHAR_TYPE_ MAX_>
 class ch_range {
  public:
   static auto exec(context& ctx_) -> generic_ast*;
 };

 template <generic_ast_base::id_type ID_, base::is_string_literal L_>
 class ch_group {
  public:
   static auto exec(context& ctx_) -> generic_ast*;
 };

 template <generic_ast_base::id_type ID_, base::is_string_literal L_>
 class ch_literal {
  public:
   static auto exec(context& ctx_) -> generic_ast*;
 };

};

}  // namespace pmt::util::parse