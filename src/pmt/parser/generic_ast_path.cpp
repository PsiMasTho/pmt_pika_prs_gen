#include "pmt/parser/generic_ast_path.hpp"

#include "pmt/base/hash.hpp"
#include "pmt/parser/generic_ast.hpp"

#include <cassert>

namespace pmt::parser {
GenericAstPath::GenericAstPath(size_t index_)
 : _path({index_}) {
}

GenericAstPath::GenericAstPath(std::vector<size_t> path_)
 : _path(std::move(path_)) {
}

auto GenericAstPath::inplace_push(size_t index_) -> GenericAstPath& {
  _path.push_back(index_);
  return *this;
}

auto GenericAstPath::inplace_push(std::vector<size_t> const& path_) -> GenericAstPath& {
  _path.insert(_path.end(), path_.begin(), path_.end());
  return *this;
}

auto GenericAstPath::inplace_push(GenericAstPath const& path_) -> GenericAstPath& {
  _path.insert(_path.end(), path_._path.begin(), path_._path.end());
  return *this;
}

auto GenericAstPath::clone_push(size_t index_) const -> GenericAstPath {
  GenericAstPath result = *this;
  result.inplace_push(index_);
  return result;
}

auto GenericAstPath::clone_push(std::vector<size_t> const& path_) const -> GenericAstPath {
  GenericAstPath result = *this;
  result.inplace_push(path_);
  return result;
}

auto GenericAstPath::clone_push(GenericAstPath const& path_) const -> GenericAstPath {
  GenericAstPath result = *this;
  result.inplace_push(path_);
  return result;
}

auto GenericAstPath::inplace_pop() -> GenericAstPath& {
  if (!_path.empty()) {
    _path.pop_back();
  }

  return *this;
}

auto GenericAstPath::clone_pop() const -> GenericAstPath {
  GenericAstPath result = *this;
  result.inplace_pop();
  return result;
}

auto GenericAstPath::inplace_clear() -> GenericAstPath& {
  _path.clear();
  return *this;
}

auto GenericAstPath::clone_clear() const -> GenericAstPath {
  GenericAstPath result = *this;
  result.inplace_clear();
  return result;
}

auto GenericAstPath::size() const -> size_t {
  return _path.size();
}

auto GenericAstPath::empty() const -> bool {
  return _path.empty();
}

auto GenericAstPath::front() const -> size_t {
  assert(!_path.empty());
  return _path.front();
}

auto GenericAstPath::back() const -> size_t {
 assert(!_path.empty());
 return _path.back();
}

auto GenericAstPath::resolve(GenericAst const& root_) const -> GenericAst const* {
  GenericAst const* ret = &root_;
  for (size_t i = 0; i < _path.size() && ret != nullptr; ++i) {
    ret = ret->get_child_at(_path[i]);
  }
  return ret;
}

auto GenericAstPath::resolve(GenericAst& root_) const -> GenericAst* {
  GenericAst* ret = &root_;
  for (size_t i = 0; i < _path.size() && ret != nullptr; ++i) {
    ret = ret->get_child_at(_path[i]);
  }
  return ret;
}

auto GenericAstPath::hash() const -> size_t {
  size_t seed = pmt::base::Hash::Phi64;
  for (size_t const index : _path) {
    pmt::base::Hash::combine(index, seed);
  }
  return seed;
}

}  // namespace pmt::util::smrt