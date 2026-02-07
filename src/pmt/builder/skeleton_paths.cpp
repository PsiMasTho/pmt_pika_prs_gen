#include "pmt/builder/skeleton_paths.hpp"

#include <filesystem>
#include <stdexcept>

#ifndef PMT_SKEL_INSTALL_DIR
#define PMT_SKEL_INSTALL_DIR ""
#endif

namespace pmt::builder {

void SkeletonPaths::set_root_override(std::string root_) {
 _root_override = std::move(root_);
}

auto SkeletonPaths::resolve(std::string_view relative_) const -> std::string {
 std::filesystem::path root = get_root();
 return (root / relative_).string();
}

auto SkeletonPaths::get_root() const -> std::string {
 if (!_root_override.empty()) {
  return _root_override;
 }
 std::string const install_dir = PMT_SKEL_INSTALL_DIR;
 if (install_dir.empty()) {
  throw std::runtime_error("Skeleton root not configured; set --skel-dir or install skeleton files.");
 }
 return install_dir;
}

}  // namespace pmt::builder
