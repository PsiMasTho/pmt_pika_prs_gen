#pragma once

#include <string>
#include <string_view>

namespace pmt::pika::builder {

class SkeletonPaths {
 std::string _root_override;

public:
 void set_root_override(std::string root_);
 auto resolve(std::string_view relative_) const -> std::string;

private:
 auto get_root() const -> std::string;
};

}  // namespace pmt::pika::builder
