#pragma once

#include <span>
#include <vector>

namespace pmt::rt {
class Ast;

class AstPath {
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
 AstPath() = default;
 explicit AstPath(IndexType index_);
 explicit AstPath(std::span<IndexType> path_);

 // --$ Access $--
 auto inplace_push(IndexType index_) -> AstPath&;
 auto inplace_push(std::span<IndexType> const& path_) -> AstPath&;
 auto inplace_push(AstPath const& path_) -> AstPath&;

 auto clone_push(IndexType index_) const -> AstPath;
 auto clone_push(std::span<IndexType> const& path_) const -> AstPath;
 auto clone_push(AstPath const& path_) const -> AstPath;

 auto inplace_pop() -> AstPath&;
 auto clone_pop() const -> AstPath;

 auto inplace_clear() -> AstPath&;
 auto clone_clear() const -> AstPath;

 auto size() const -> size_t;
 auto empty() const -> bool;
 auto front() const -> IndexType;
 auto back() const -> IndexType;

 // --$ Resolution $--
 auto resolve(Ast const& root_) const -> Ast const*;
 auto resolve(Ast& root_) const -> Ast*;

 // --$ Hashing $--
 auto hash() const -> size_t;

 // --$ Comparison $--
 auto operator==(AstPath const& other_) const -> bool = default;
};

}  // namespace pmt::rt