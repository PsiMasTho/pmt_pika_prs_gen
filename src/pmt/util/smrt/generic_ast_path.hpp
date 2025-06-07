#pragma once

#include "pmt/base/hashable.hpp"
#include "pmt/fw_decl.hpp"

#include <vector>

PMT_FW_DECL_NS_CLASS(pmt::util::smrt, GenericAst)

namespace pmt::util::smrt {

class GenericAstPath : public pmt::base::Hashable<GenericAstPath> {
 public:
  // - Lifetime -
  GenericAstPath() = default;
  explicit GenericAstPath(size_t index_);
  explicit GenericAstPath(std::vector<size_t> path_);

  // - Access -
  auto inplace_push(size_t index_) -> GenericAstPath&;
  auto inplace_push(std::vector<size_t> const& path_) -> GenericAstPath&;
  auto inplace_push(GenericAstPath const& path_) -> GenericAstPath&;

  auto clone_push(size_t index_) const -> GenericAstPath;
  auto clone_push(std::vector<size_t> const& path_) const -> GenericAstPath;
  auto clone_push(GenericAstPath const& path_) const -> GenericAstPath;

  auto inplace_pop() -> GenericAstPath&;
  auto clone_pop() const -> GenericAstPath;

  auto inplace_clear() -> GenericAstPath&;
  auto clone_clear() const -> GenericAstPath;

  auto size() const -> size_t;
  auto empty() const -> bool;
  auto back() const -> size_t;

  // - Resolution -
  auto resolve(GenericAst const& root_) const -> GenericAst const*;
  auto resolve(GenericAst& root_) const -> GenericAst*;

  // - pmt::base::Hashable -
  auto hash() const -> size_t;

  // - Comparison -
  auto operator==(GenericAstPath const& other_) const -> bool = default;

 private:
  std::vector<size_t> _path;
};

}  // namespace pmt::util::smrt