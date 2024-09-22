// clang-format off
#ifdef __INTELLISENSE__
    #include "pmt/util/text_stream.hpp"
#endif
// clang-format on

namespace pmt::util {

template <text_encoding ENCODING_>
auto text_stream::construct(typename text_encoding_traits<ENCODING_>::string_view_type str_) -> text_stream {
  return text_stream{reinterpret_cast<unsigned char const*>(str_.data()), reinterpret_cast<unsigned char const*>(str_.data() + str_.size()), ENCODING_};
}

}  // namespace pmt::util