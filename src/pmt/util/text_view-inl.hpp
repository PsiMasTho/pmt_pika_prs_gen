// clang-format off
#ifdef __INTELLISENSE__
    #include "pmt/util/text_view.hpp"
#endif
// clang-format on

#include <cassert>

namespace pmt::util {

template <typename CHAR_TYPE_>
text_view::text_view(std::basic_string_view<CHAR_TYPE_> str_, text_encoding encoding_)
 : text_view(reinterpret_cast<unsigned char const*>(str_.data()), reinterpret_cast<unsigned char const*>(str_.data() + str_.size()), encoding_) {
  assert(_encoding._byte_count == sizeof(CHAR_TYPE_));
}

}  // namespace pmt::util