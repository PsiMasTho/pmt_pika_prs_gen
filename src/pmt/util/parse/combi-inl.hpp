// clang-format off
#ifdef __INTELLISENSE__
    #include "pmt/util/parse/combi.hpp"
#endif
// clang-format on

namespace pmt::util::parse {

template <text_encoding ENCODING_, typename T_>
concept is_combi_hide = combi<ENCODING_>::template is_hide<T_>::value;

template <text_encoding ENCODING_, typename T_>
concept is_combi_unpack = combi<ENCODING_>::template is_unpack<T_>::value;

template <text_encoding ENCODING_>
template <generic_ast_base::id_type ID_, typename... TS_>
auto combi<ENCODING_>::seq<ID_, TS_...>::exec(text_stream& stream_) -> generic_ast* {
  typename generic_ast::unique_handle result = generic_ast::construct(generic_ast_base::TAG_CHILDREN);
  result->set_id(ID_);
  auto const backtrack = stream_.get_position();

  bool success = true;

  auto const do_exec = [&]<typename T_>() -> bool {
    typename generic_ast::unique_handle child{T_::exec(stream_), generic_ast::destruct};

    if (!child)
      return success = false;

    if constexpr (is_combi_unpack<ENCODING_, T_>) {
      // Unpack the children OR just insert the token
      if (child->get_tag() == generic_ast_base::TAG_TOKEN) {
        result->set_token(child->get_token());
      } else {
        while (child->get_children_size() > 0)
          result->give_child_at(result->get_children_size(), child->take_child_at(0));
      }
    } else if constexpr (is_combi_hide<ENCODING_, T_>) {
      // Do nothing
    } else {
      result->give_child_at(result->get_children_size(), std::move(child));
    }

    return true;
  };

  (... && do_exec.template operator()<TS_>());

  if (!success) {
    stream_.set_position(backtrack);
    return nullptr;
  }

  return result.release();
}

template <text_encoding ENCODING_>
template <generic_ast_base::id_type ID_, typename... TS_>
auto combi<ENCODING_>::sor<ID_, TS_...>::exec(text_stream& stream_) -> generic_ast* {
  typename generic_ast::unique_handle result = generic_ast::construct(generic_ast_base::TAG_CHILDREN);
  result->set_id(ID_);
  auto const backtrack = stream_.get_position();

  bool success = false;

  auto const do_exec = [&]<typename T_>() -> bool {
    typename generic_ast::unique_handle child{T_::exec(stream_), generic_ast::destruct};

    if (!child)
      return false;

    if constexpr (is_combi_unpack<ENCODING_, T_>) {
      // Unpack the children OR just insert the token
      if (child->get_tag() == generic_ast_base::TAG_TOKEN) {
        result->set_token(child->get_token());
      } else {
        while (child->get_children_size() > 0)
          result->give_child_at(result->get_children_size(), child->take_child_at(0));
      }
    } else if constexpr (is_combi_hide<ENCODING_, T_>) {
      // Do nothing
    } else {
      result->give_child_at(result->get_children_size(), std::move(child));
    }

    return success = true;
  };

  (... || do_exec.template operator()<TS_>());

  if (!success) {
    stream_.set_position(backtrack);
    return nullptr;
  }

  return result.release();
}

template <text_encoding ENCODING_>
template <generic_ast_base::id_type ID_, typename T_>
auto combi<ENCODING_>::star<ID_, T_>::exec(text_stream& stream_) -> generic_ast* {
  typename generic_ast::unique_handle result = generic_ast::construct(generic_ast_base::TAG_CHILDREN);
  result->set_id(ID_);
  auto backtrack = stream_.get_position();

  auto const do_iterate = [&]() -> bool {
    typename generic_ast::unique_handle child{T_::exec(stream_), generic_ast::destruct};

    if (!child) {
      return false;
    }

    if constexpr (is_combi_unpack<ENCODING_, T_>) {
      // Unpack the children OR just insert the token
      if (child->get_tag() == generic_ast_base::TAG_TOKEN) {
        result->set_token(child->get_token());
      } else {
        while (child->get_children_size() > 0)
          result->give_child_at(result->get_children_size(), child->take_child_at(0));
      }
    } else if constexpr (is_combi_hide<ENCODING_, T_>) {
      // Do nothing
    } else {
      result->give_child_at(result->get_children_size(), std::move(child));
    }

    backtrack = stream_.get_position();
    return true;
  };

  while (do_iterate()) {
    // nothing
  }

  stream_.set_position(backtrack);

  return result.release();
}

template <text_encoding ENCODING_>
template <generic_ast_base::id_type ID_, typename T_>
auto combi<ENCODING_>::plus<ID_, T_>::exec(text_stream& stream_) -> generic_ast* {
  return seq<ID_, T_, unpack<star<T_::_id, T_>>>::exec(stream_);
}

template <text_encoding ENCODING_>
template <generic_ast_base::id_type ID_, typename T_>
auto combi<ENCODING_>::opt<ID_, T_>::exec(text_stream& stream_) -> generic_ast* {
  typename generic_ast::unique_handle result = generic_ast::construct(generic_ast_base::TAG_CHILDREN);
  result->set_id(ID_);
  auto const backtrack = stream_.get_position();

  typename generic_ast::unique_handle child{T_::exec(stream_), generic_ast::destruct};

  if (!child) {
    stream_.set_position(backtrack);
    return nullptr;
  }

  if constexpr (is_combi_unpack<ENCODING_, T_>) {
    // Unpack the children OR just insert the token
    if (child->get_tag() == generic_ast_base::TAG_TOKEN) {
      result->set_token(child->get_token());
    } else {
      for (size_t i = 0; i < child->get_children_size(); ++i)
        result->give_child_at(result->get_children_size(), child->take_child_at(i));
    }
  } else if constexpr (is_combi_hide<ENCODING_, T_>) {
    // Do nothing
  } else {
    result->give_child_at(result->get_children_size(), std::move(child));
  }

  return result.release();
}

template <text_encoding ENCODING_>
template <generic_ast_base::id_type ID_, codepoint_type C_>
auto combi<ENCODING_>::ch<ID_, C_>::exec(text_stream& stream_) -> generic_ast* {
  if (stream_.is_at_end())
    return nullptr;
  
  auto const backtrack = stream_.get_position();
  if (stream_.read() != C_) {
    stream_.set_position(backtrack);
    return nullptr;
  }

  typename generic_ast::unique_handle result = generic_ast::construct(generic_ast_base::TAG_TOKEN);
  result->set_id(ID_);
  result->set_token({C_});

  return result.release();
}

template <text_encoding ENCODING_>
template <generic_ast_base::id_type ID_, codepoint_type MIN_, codepoint_type MAX_>
auto combi<ENCODING_>::ch_range<ID_, MIN_, MAX_>::exec(text_stream& stream_) -> generic_ast* {
  if (stream_.is_at_end())
    return nullptr;

  auto const backtrack = stream_.get_position();
  codepoint_type const codepoint = stream_.read();

  if (codepoint < MIN_ || codepoint > MAX_) {
    stream_.set_position(backtrack);
    return nullptr;
  }

  typename generic_ast::unique_handle result = generic_ast::construct(generic_ast_base::TAG_TOKEN);
  result->set_id(ID_);
  result->set_token({codepoint});

  return result.release();
}

template <text_encoding ENCODING_>
template <typename T_>
auto combi<ENCODING_>::merge<T_>::exec(text_stream& stream_) -> generic_ast* {
  typename generic_ast::unique_handle child{T_::exec(stream_), generic_ast::destruct};

  if (!child)
    return nullptr;

  child->merge();

  return child.release();
}

template <text_encoding ENCODING_>
template <typename T_>
auto combi<ENCODING_>::hide<T_>::exec(text_stream& stream_) -> generic_ast* {
  return T_::exec(stream_);
}

template <text_encoding ENCODING_>
template <typename T_>
auto combi<ENCODING_>::unpack<T_>::exec(text_stream& stream_) -> generic_ast* {
  return T_::exec(stream_);
}

}  // namespace pmt::util::parse
