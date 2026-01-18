#pragma once

#include "pmt/util/singleton.hpp"

#include <string>
#include <string_view>

namespace pmt::util {

class SkeletonPaths {
 std::string _root_override;

 public:
 void set_root_override(std::string root_);
 auto get_path(std::string_view relative_) const -> std::string;

private:
 auto get_root() const -> std::string;

};

using SkeletonPathsSingleton = Singleton<SkeletonPaths>;

}  // namespace pmt::util
