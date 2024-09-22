#pragma once

#include "pmt/util/parse/generic_ast.hpp"
#include "pmt/util/text_encoding.hpp"
#include "pmt/util/text_stream.hpp"

namespace pmt::util::parse {

template <text_encoding ENCODING_>
class combi {
 public:
  using generic_ast = generic_ast<ENCODING_>;
  
 //-- Combinators -------------------------------------------------------------
 // -- Sequence --
  template <generic_ast_base::id_type ID_, typename... TS_>
  class seq {
   public:
    static const auto _id = ID_;
    static auto exec(text_stream& stream_) -> generic_ast*;
  };

  template <typename... TS_>
  class seq<generic_ast_base::DEFAULT_ID, TS_...>;

  // -- Ordered choice --
  template <generic_ast_base::id_type ID_, typename... TS_>
  class sor {
   public:
    static const auto _id = ID_;
    static auto exec(text_stream& stream_) -> generic_ast*;
  };

  template <typename... TS_>
  class sor<generic_ast_base::DEFAULT_ID, TS_...>;

  // -- Star (Zero or more) --
  template <generic_ast_base::id_type ID_, typename T_>
  class star {
   public:
    static const auto _id = ID_;
    static auto exec(text_stream& stream_) -> generic_ast*;
  };

  template <typename T_>
  class star<generic_ast_base::DEFAULT_ID, T_>;

  // -- Plus (One or more) --
  template <generic_ast_base::id_type ID_, typename T_>
  class plus {
   public:
    static const auto _id = ID_;
    static auto exec(text_stream& stream_) -> generic_ast*;
  };

  template <typename T_>
  class plus<generic_ast_base::DEFAULT_ID, T_>;

  // -- Optional (One or none) --
  template <generic_ast_base::id_type ID_, typename T_>
  class opt {
   public:
    static const auto _id = ID_;
    static auto exec(text_stream& stream_) -> generic_ast*;
  };

  template <typename T_>
  class opt<generic_ast_base::DEFAULT_ID, T_>;

  //-- Character matching ------------------------------------------------------
  // -- Single character --
  template <generic_ast_base::id_type ID_, codepoint_type C_>
  class ch {
   public:
    static const auto _id = ID_;
    static auto exec(text_stream& stream_) -> generic_ast*;
  };

  template <codepoint_type C_>
  class ch<generic_ast_base::DEFAULT_ID, C_>;

  // -- Character range --
  template <generic_ast_base::id_type ID_, codepoint_type MIN_, codepoint_type MAX_>
  class ch_range {
    static_assert(MIN_ <= MAX_);

   public:
    static const auto _id = ID_;
    static auto exec(text_stream& stream_) -> generic_ast*;
  };

  template <codepoint_type MIN_, codepoint_type MAX_>
  class ch_range<generic_ast_base::DEFAULT_ID, MIN_, MAX_>;

  //-- Ast shaping -------------------------------------------------------------
  /// The return value of @tparam T_, if it is children, is recursively
  /// transformed into a single token.
  template <typename T_>
  class merge {
   public:
    static const auto _id = T_::_id;
    static auto exec(text_stream& stream_) -> generic_ast*;
  };

  /// The return value of @tparam T_ is not added to the result of
  /// any combinator that has this as a child.
  template <typename T_>
  class hide {
   public:
    static const auto _id = T_::_id;
    static auto exec(text_stream& stream_) -> generic_ast*;
  };

  /// The return value of @tparam T_, if it is children, is added
  /// directly to the result of the combinator that has this as a child.
  template <typename T_>
  class unpack {
   public:
    static const auto _id = generic_ast_base::DEFAULT_ID;
    static auto exec(text_stream& stream_) -> generic_ast*;
  };

  private:
  //-- Type traits (internal use) ----------------------------------------------
  template <typename T_>
  class is_hide : public std::false_type {};

  template <typename T_>
  class is_hide<hide<T_>> : public std::true_type {};

  template <typename T_>
  class is_unpack : public std::false_type {};

  template <typename T_>
  class is_unpack<unpack<T_>> : public std::true_type {};
};

}  // namespace pmt::util::parse

#include "pmt/util/parse/combi/combi-inl.hpp"