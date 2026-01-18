#include "pmt/util/skeleton_paths.hpp"

#include <filesystem>
#include <stdexcept>

#ifndef PMT_SKEL_INSTALL_DIR
#define PMT_SKEL_INSTALL_DIR ""
#endif

namespace pmt::util {
namespace {

std::string g_skeleton_root_override;

auto get_skeleton_root() -> std::string {
 if (!g_skeleton_root_override.empty()) {
  return g_skeleton_root_override;
 }
 std::string const install_dir = PMT_SKEL_INSTALL_DIR;
 if (install_dir.empty()) {
  throw std::runtime_error("Skeleton root not configured; set --skel-dir or install skeleton files.");
 }
 return install_dir;
}

}  // namespace

void set_skeleton_root_override(std::string root_) {
 g_skeleton_root_override = std::move(root_);
}

auto get_skeleton_path(std::string_view relative_) -> std::string {
 std::filesystem::path root = get_skeleton_root();
 return (root / relative_).string();
}

}  // namespace pmt::util
