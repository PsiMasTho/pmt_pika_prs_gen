#pragma once

#include <span>
#include <vector>

namespace pmt::rt {
class Ast;

class Path {
public:
 // -$ Types / Constants $-
 using IndexType = size_t;

private:
 using ContainerType = std::vector<IndexType>;

 // -$ Data $-
 ContainerType _path;

public:
 // -$ Functions $-
 // --$ Lifetime $--
 Path() = default;
 explicit Path(IndexType index_);
 explicit Path(std::span<IndexType> path_);

 // --$ Operators $--
 auto operator==(Path const& other_) const -> bool = default;

 // --$ Other $--
 auto inplace_push(IndexType index_) -> Path&;
 auto inplace_push(std::span<IndexType> const& path_) -> Path&;
 auto inplace_push(Path const& path_) -> Path&;

 auto clone_push(IndexType index_) const -> Path;
 auto clone_push(std::span<IndexType> const& path_) const -> Path;
 auto clone_push(Path const& path_) const -> Path;

 auto inplace_pop() -> Path&;
 auto clone_pop() const -> Path;

 auto inplace_clear() -> Path&;
 auto clone_clear() const -> Path;

 auto size() const -> size_t;
 auto empty() const -> bool;
 auto front() const -> IndexType;
 auto back() const -> IndexType;

 auto resolve(Ast const& root_) const -> Ast const*;
 auto resolve(Ast& root_) const -> Ast*;
};

}  // namespace pmt::rt