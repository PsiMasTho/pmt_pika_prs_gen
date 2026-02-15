#include "pmt/pika/rt/path.hpp"

#include "pmt/pika/rt/ast.hpp"

#include <cassert>

namespace pmt::pika::rt {
Path::Path(IndexType index_)
 : _path({index_}) {
}

Path::Path(std::span<IndexType> path_)
 : _path(path_.begin(), path_.end()) {
}

auto Path::inplace_push(IndexType index_) -> Path& {
 _path.push_back(index_);
 return *this;
}

auto Path::inplace_push(std::span<IndexType> const& path_) -> Path& {
 _path.insert(_path.end(), path_.begin(), path_.end());
 return *this;
}

auto Path::inplace_push(Path const& path_) -> Path& {
 _path.insert(_path.end(), path_._path.begin(), path_._path.end());
 return *this;
}

auto Path::clone_push(IndexType index_) const -> Path {
 Path result = *this;
 result.inplace_push(index_);
 return result;
}

auto Path::clone_push(std::span<IndexType> const& path_) const -> Path {
 Path result = *this;
 result.inplace_push(path_);
 return result;
}

auto Path::clone_push(Path const& path_) const -> Path {
 Path result = *this;
 result.inplace_push(path_);
 return result;
}

auto Path::inplace_pop() -> Path& {
 if (!_path.empty()) {
  _path.pop_back();
 }

 return *this;
}

auto Path::clone_pop() const -> Path {
 Path result = *this;
 result.inplace_pop();
 return result;
}

auto Path::inplace_clear() -> Path& {
 _path.clear();
 return *this;
}

auto Path::clone_clear() const -> Path {
 Path result = *this;
 result.inplace_clear();
 return result;
}

auto Path::size() const -> size_t {
 return _path.size();
}

auto Path::empty() const -> bool {
 return _path.empty();
}

auto Path::front() const -> IndexType {
 assert(!_path.empty());
 return _path.front();
}

auto Path::back() const -> IndexType {
 assert(!_path.empty());
 return _path.back();
}

auto Path::resolve(Ast const& root_) const -> Ast const* {
 Ast const* ret = &root_;
 for (size_t i = 0; i < _path.size() && ret != nullptr; ++i) {
  ret = ret->get_child_at(_path[i]);
 }
 return ret;
}

auto Path::resolve(Ast& root_) const -> Ast* {
 Ast* ret = &root_;
 for (size_t i = 0; i < _path.size() && ret != nullptr; ++i) {
  ret = ret->get_child_at(_path[i]);
 }
 return ret;
}

}  // namespace pmt::pika::rt