#include "pmt/rt/ast_path.hpp"

#include "pmt/hash.hpp"
#include "pmt/rt/ast.hpp"

#include <cassert>

namespace pmt::rt {
AstPath::AstPath(IndexType index_)
 : _path({index_}) {
}

AstPath::AstPath(std::span<IndexType> path_)
 : _path(path_.begin(), path_.end()) {
}

auto AstPath::inplace_push(IndexType index_) -> AstPath& {
 _path.push_back(index_);
 return *this;
}

auto AstPath::inplace_push(std::span<IndexType> const& path_) -> AstPath& {
 _path.insert(_path.end(), path_.begin(), path_.end());
 return *this;
}

auto AstPath::inplace_push(AstPath const& path_) -> AstPath& {
 _path.insert(_path.end(), path_._path.begin(), path_._path.end());
 return *this;
}

auto AstPath::clone_push(IndexType index_) const -> AstPath {
 AstPath result = *this;
 result.inplace_push(index_);
 return result;
}

auto AstPath::clone_push(std::span<IndexType> const& path_) const -> AstPath {
 AstPath result = *this;
 result.inplace_push(path_);
 return result;
}

auto AstPath::clone_push(AstPath const& path_) const -> AstPath {
 AstPath result = *this;
 result.inplace_push(path_);
 return result;
}

auto AstPath::inplace_pop() -> AstPath& {
 if (!_path.empty()) {
  _path.pop_back();
 }

 return *this;
}

auto AstPath::clone_pop() const -> AstPath {
 AstPath result = *this;
 result.inplace_pop();
 return result;
}

auto AstPath::inplace_clear() -> AstPath& {
 _path.clear();
 return *this;
}

auto AstPath::clone_clear() const -> AstPath {
 AstPath result = *this;
 result.inplace_clear();
 return result;
}

auto AstPath::size() const -> size_t {
 return _path.size();
}

auto AstPath::empty() const -> bool {
 return _path.empty();
}

auto AstPath::front() const -> IndexType {
 assert(!_path.empty());
 return _path.front();
}

auto AstPath::back() const -> IndexType {
 assert(!_path.empty());
 return _path.back();
}

auto AstPath::resolve(Ast const& root_) const -> Ast const* {
 Ast const* ret = &root_;
 for (size_t i = 0; i < _path.size() && ret != nullptr; ++i) {
  ret = ret->get_child_at(_path[i]);
 }
 return ret;
}

auto AstPath::resolve(Ast& root_) const -> Ast* {
 Ast* ret = &root_;
 for (size_t i = 0; i < _path.size() && ret != nullptr; ++i) {
  ret = ret->get_child_at(_path[i]);
 }
 return ret;
}

auto AstPath::hash() const -> size_t {
 size_t seed = pmt::Hash::Phi64;
 for (IndexType const index : _path) {
  pmt::Hash::combine(index, seed);
 }
 return seed;
}

}  // namespace pmt::rt