// clang-format off
#ifdef __INTELLISENSE__
    #include "pmt/util/parse/combi.hpp"
#endif
// clang-format on

namespace pmt::util::parse {

template <typename CHAR_TYPE_>
template <generic_ast_base::id_type ID_, typename... TS_>
auto combi<CHAR_TYPE_>::seq<ID_, TS_...>::exec(context& ctx_) -> generic_ast* {
  typename generic_ast::unique_handle result{generic_ast::construct(), generic_ast::destruct};
  result->set_id(ID_);
  CHAR_TYPE_ const* backtrack = ctx_._cursor;

  bool success = true;

  auto const do_exec = [&]<typename T_>() -> bool {
    typename generic_ast::unique_handle child{T_::exec(ctx_), generic_ast::destruct};

    if (!child)
      return success = false;

    if constexpr (is_combi_unpack<CHAR_TYPE_, T_>) {
      // Unpack the children OR just insert the token
      if (child->is_token()) {
        result->set_token(child->get_token());
      } else {
        for (size_t i = 0; i < child->get_children_size(); ++i)
          result->insert_child_at(result->get_children_size(), child->take_child_at(i).release());
      }
    } else if constexpr (is_combi_hide<CHAR_TYPE_, T_>) {
      // Do nothing
    } else {
      result->insert_child_at(result->get_children_size(), child.release());
    }

    return true;
  };

  (... && do_exec.template operator()<TS_>());

  if (!success) {
    ctx_._cursor = backtrack;
    return nullptr;
  }

  return result.release();
}

template <typename CHAR_TYPE_>
template <generic_ast_base::id_type ID_, typename... TS_>
auto combi<CHAR_TYPE_>::sor<ID_, TS_...>::exec(context& ctx_) -> generic_ast* {
  typename generic_ast::unique_handle result{generic_ast::construct(), generic_ast::destruct};
  result->set_id(ID_);
  CHAR_TYPE_ const* backtrack = ctx_._cursor;

  bool success = false;

  auto const do_exec = [&]<typename T_>() -> bool {
    typename generic_ast::unique_handle child{T_::exec(ctx_), generic_ast::destruct};

    if (!child)
      return false;

    if constexpr (is_combi_unpack<CHAR_TYPE_, T_>) {
      // Unpack the children OR just insert the token
      if (child->is_token()) {
        result->set_token(child->get_token());
      } else {
        for (size_t i = 0; i < child->get_children_size(); ++i)
          result->insert_child_at(result->get_children_size(), child->take_child_at(i).release());
      }
    } else if constexpr (is_combi_hide<CHAR_TYPE_, T_>) {
      // Do nothing
    } else {
      result->insert_child_at(result->get_children_size(), child.release());
    }

    return success = true;
  };

  (... || do_exec.template operator()<TS_>());

  if (!success) {
    ctx_._cursor = backtrack;
    return nullptr;
  }

  return result.release();
}

template <typename CHAR_TYPE_>
template <generic_ast_base::id_type ID_, typename T_>
auto combi<CHAR_TYPE_>::star<ID_, T_>::exec(context& ctx_) -> generic_ast* {
  typename generic_ast::unique_handle result{generic_ast::construct(), generic_ast::destruct};
  result->set_id(ID_);
  CHAR_TYPE_ const* backtrack = ctx_._cursor;

  auto const do_iterate = [&]() -> bool {
    typename generic_ast::unique_handle child{T_::exec(ctx_), generic_ast::destruct};

    if (!child) {
      return false;
    }

    if constexpr (is_combi_unpack<CHAR_TYPE_, T_>) {
      // Unpack the children OR just insert the token
      if (child->is_token()) {
        result->set_token(child->get_token());
      } else {
        for (size_t i = 0; i < child->get_children_size(); ++i)
          result->insert_child_at(result->get_children_size(), child->take_child_at(i).release());
      }
    } else if constexpr (is_combi_hide<CHAR_TYPE_, T_>) {
      // Do nothing
    } else {
      result->insert_child_at(result->get_children_size(), child.release());
    }

    backtrack = ctx_._cursor;
    return true;
  };

  while (do_iterate()) {
    // nothing
  }

  ctx_._cursor = backtrack;

  return result.release();
}

template <typename CHAR_TYPE_>
template <generic_ast_base::id_type ID_, typename T_>
auto combi<CHAR_TYPE_>::plus<ID_, T_>::exec(context& ctx_) -> generic_ast* {
  return seq<ID_, T_, unpack<star<T_::_id, T_>>>(ctx_);
}

template <typename CHAR_TYPE_>
template <generic_ast_base::id_type ID_, typename T_>
auto combi<CHAR_TYPE_>::opt<ID_, T_>::exec(context& ctx_) -> generic_ast* {
  typename generic_ast::unique_handle result{generic_ast::construct(), generic_ast::destruct};
  result->set_id(ID_);
  CHAR_TYPE_ const* backtrack = ctx_._cursor;

  typename generic_ast::unique_handle child{T_::exec(ctx_), generic_ast::destruct};

  if (!child) {
    ctx_._cursor = backtrack;
    return nullptr;
  }

  if constexpr (is_combi_unpack<CHAR_TYPE_, T_>) {
    // Unpack the children OR just insert the token
    if (child->is_token()) {
      result->set_token(child->get_token());
    } else {
      for (size_t i = 0; i < child->get_children_size(); ++i)
        result->insert_child_at(result->get_children_size(), child->take_child_at(i).release());
    }
  } else if constexpr (is_combi_hide<CHAR_TYPE_, T_>) {
    // Do nothing
  } else {
    result->insert_child_at(result->get_children_size(), child.release());
  }

  return result.release();
}

template <typename CHAR_TYPE_>
template <generic_ast_base::id_type ID_, CHAR_TYPE_ C_>
auto combi<CHAR_TYPE_>::ch<ID_, C_>::exec(context& ctx_) -> generic_ast* {
  if (ctx_._cursor == ctx_._end || *ctx_._cursor != C_)
    return nullptr;

  typename generic_ast::unique_handle result{generic_ast::construct(), generic_ast::destruct};
  result->set_id(ID_);
  result->set_token({C_});

  ++ctx_._cursor;

  return result.release();
}

template <typename CHAR_TYPE_>
template <generic_ast_base::id_type ID_, CHAR_TYPE_ MIN_, CHAR_TYPE_ MAX_>
auto combi<CHAR_TYPE_>::ch_range<ID_, MIN_, MAX_>::exec(context& ctx_) -> generic_ast* {
  if (ctx_._cursor == ctx_._end || *ctx_._cursor < MIN_ || *ctx_._cursor > MAX_)
    return nullptr;

  typename generic_ast::unique_handle result{generic_ast::construct(), generic_ast::destruct};
  result->set_id(ID_);
  result->set_token({*ctx_._cursor});

  ++ctx_._cursor;

  return result.release();
}

template <typename CHAR_TYPE_>
template <typename T_>
auto combi<CHAR_TYPE_>::merge<T_>::exec(context& ctx_) -> generic_ast* {
  typename generic_ast::unique_handle child{T_::exec(ctx_), generic_ast::destruct};

  if (!child)
    return nullptr;

  child->merge();

  return child.release();
}

template <typename CHAR_TYPE_>
template <typename T_>
auto combi<CHAR_TYPE_>::hide<T_>::exec(context& ctx_) -> generic_ast* {
  return T_::exec(ctx_);
}

template <typename CHAR_TYPE_>
template <typename T_>
auto combi<CHAR_TYPE_>::unpack<T_>::exec(context& ctx_) -> generic_ast* {
  return T_::exec(ctx_);
}

}  // namespace pmt::util::parse
