#include "pmt/util/skeleton_replacer_base.hpp"

namespace pmt::util {

auto SkeletonReplacerBase::HeterogeneousHasher::operator()(std::string_view str_) const -> size_t {
 return std::hash<std::string_view>{}(str_);
}

auto SkeletonReplacerBase::HeterogeneousHasher::operator()(std::string const& str_) const -> size_t {
 return std::hash<std::string>{}(str_);
}

void SkeletonReplacerBase::replace_skeleton_label(std::string& dest_, std::string_view label_, std::string const& replacement_) {
  auto itr = _label_to_regex_map.find(label_);
  if (itr == _label_to_regex_map.end()) {
   std::string label(label_);
   std::string const pattern = R"(\/\*\s*\$replace\s*)" + label + R"(\s*\*\/)";
   itr = _label_to_regex_map.emplace(std::make_pair<>(std::move(label), std::regex(pattern, std::regex::optimize))).first;
  }
  dest_ = std::regex_replace(dest_, itr->second, replacement_);
}

}