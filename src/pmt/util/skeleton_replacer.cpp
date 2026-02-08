#include "pmt/util/skeleton_replacer.hpp"

namespace pmt::util {

auto SkeletonReplacer::HeterogeneousHasher::operator()(std::string_view str_) const -> size_t {
 return std::hash<std::string_view>{}(str_);
}

auto SkeletonReplacer::HeterogeneousHasher::operator()(std::string const& str_) const -> size_t {
 return std::hash<std::string>{}(str_);
}

void SkeletonReplacer::replace_label(std::string& dest_, std::string_view label_, std::string const& replacement_) {
 auto itr = _label_to_regex_map.find(label_);

 if (itr == _label_to_regex_map.end()) {
  std::string label(label_);
  std::string const pattern = R"(\/\*\s*\$replace\s*)" + label + R"(\s*\*\/)";
  itr = _label_to_regex_map.emplace(std::make_pair<>(std::move(label), std::regex(pattern, std::regex::optimize))).first;
 }

 // Note: using std::regex_replace naively doesnt work because the actual destination string is interpreted as a format string.
 //       So we have to do it manually.
 std::smatch match;
 size_t search_from = 0;

 while (std::regex_search(dest_.cbegin() + search_from, dest_.cend(), match, itr->second)) {
  dest_.replace(match.position(0) + search_from, match.length(0), replacement_);
  search_from += match.position(0) + replacement_.size();
 }
}

}  // namespace pmt::util