#pragma once

#include "pmt/util/generic_ast.hpp"

#include <string_view>

namespace pmt::util::parse {

template <generic_ast_base::id_type ID_>
class with_id {
 public:
  static const auto _id = ID_;
};

namespace detail {
namespace trait {
template <typename T_>
class is_with_id : public std::false_type {};

template <generic_ast_base::id_type ID_>
class is_with_id<with_id<ID_>> : public std::true_type {};
}  // namespace trait
namespace ccpt {
template <typename T_>
concept is_with_id = trait::is_with_id<T_>::value;
}

}  // namespace detail

template <typename CHAR_TYPE_>
class combi {
 public:
  using generic_ast = generic_ast<CHAR_TYPE_>;
  class context;
  //-- Implementations ----------------------------------------------------------
 private:
  template <generic_ast_base::id_type ID_, typename... TS_>
  class seq_impl {
   public:
    static const auto _id = ID_;
    static auto exec(context& ctx_) -> generic_ast*;
  };

  //-- Combinators -------------------------------------------------------------
 public:
 // -- Sequence --
  template <typename... TS_>
  class seq;

  template <typename... TS_>
  class seq : public seq_impl<generic_ast_base::DEFAULT_ID, TS_...> {};

  template <detail::ccpt::is_with_id T_, typename... TS_>
  class seq<T_, TS_...> : public seq_impl<T_::_id, TS_...> {};

  // -- Ordered choice --
  template <generic_ast_base::id_type ID_, typename... TS_>
  class sor {
   public:
    static const auto _id = ID_;
    static auto exec(context& ctx_) -> generic_ast*;
  };

  template <generic_ast_base::id_type ID_, typename T_>
  class star {
   public:
    static const auto _id = ID_;
    static auto exec(context& ctx_) -> generic_ast*;
  };

  template <generic_ast_base::id_type ID_, typename T_>
  class plus {
   public:
    static const auto _id = ID_;
    static auto exec(context& ctx_) -> generic_ast*;
  };

  template <generic_ast_base::id_type ID_, typename T_>
  class opt {
   public:
    static const auto _id = ID_;
    static auto exec(context& ctx_) -> generic_ast*;
  };

  //-- Character parsers -------------------------------------------------------
  template <generic_ast_base::id_type ID_, CHAR_TYPE_ C_>
  class ch {
   public:
    static const auto _id = ID_;
    static auto exec(context& ctx_) -> generic_ast*;
  };

  template <generic_ast_base::id_type ID_, CHAR_TYPE_ MIN_, CHAR_TYPE_ MAX_>
  class ch_range {
    static_assert(MIN_ <= MAX_);

   public:
    static const auto _id = ID_;
    static auto exec(context& ctx_) -> generic_ast*;
  };

  //-- Ast shaping -------------------------------------------------------------

  /// The return value of @tparam T_, if it is children, is recursively
  /// transformed into a single token.
  template <typename T_>
  class merge {
   public:
    static const auto _id = T_::_id;
    static auto exec(context& ctx_) -> generic_ast*;
  };

  /// The return value of @tparam T_ is not added to the result of
  /// any combinator that has this as a child.
  template <typename T_>
  class hide {
   public:
    static const auto _id = T_::_id;
    static auto exec(context& ctx_) -> generic_ast*;
  };

  /// The return value of @tparam T_, if it is children, is added
  /// directly to the result of the combinator that has this as a child.
  template <typename T_>
  class unpack {
   public:
    static const auto _id = generic_ast_base::DEFAULT_ID;
    static auto exec(context& ctx_) -> generic_ast*;
  };

  //-- Type traits -------------------------------------------------------------
  template <typename T_>
  class is_hide : public std::false_type {};

  template <typename T_>
  class is_hide<hide<T_>> : public std::true_type {};

  template <typename T_>
  class is_unpack : public std::false_type {};

  template <typename T_>
  class is_unpack<unpack<T_>> : public std::true_type {};
};

template <typename CHAR_TYPE_, typename T_>
concept is_combi_hide = combi<CHAR_TYPE_>::template is_hide<T_>::value;

template <typename CHAR_TYPE_, typename T_>
concept is_combi_unpack = combi<CHAR_TYPE_>::template is_unpack<T_>::value;

}  // namespace pmt::util::parse

#include "pmt/util/parse/combi-inl.hpp"