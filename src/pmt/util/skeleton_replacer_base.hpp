#pragma once

#include <regex>
#include <string_view>
#include <unordered_map>

namespace pmt::util {
class SkeletonReplacerBase {
 private:
  // -$ Types / Constants $-
  struct HeterogeneousHasher {
    using is_transparent = void;  // NOLINT
    [[nodiscard]] auto operator()(std::string_view str_) const -> size_t;
    [[nodiscard]] auto operator()(std::string const& str_) const -> size_t;
  };

  // -$ Data $-
  std::unordered_map<std::string, std::regex, HeterogeneousHasher, std::equal_to<>> _label_to_regex_map;

 public:
  // -$ Functions $-
  // --$ Other $--
  void replace_skeleton_label(std::string& dest_, std::string_view label_, std::string const& replacement_);
};
}  // namespace pmt::util