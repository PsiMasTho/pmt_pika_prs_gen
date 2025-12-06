#include "pmt/util/directory_watcher.hpp"

#include <array>
#include <cerrno>
#include <stdexcept>
#include <string>
#include <system_error>
#include <utility>

#include <sys/inotify.h>
#include <unistd.h>

namespace pmt::util {

namespace {
constexpr uint32_t WATCH_MASK = IN_CREATE | IN_MODIFY | IN_DELETE;
constexpr size_t EVENT_BUFFER_SIZE = 4096;
}  // namespace

DirectoryWatcher::DirectoryWatcher(std::filesystem::path directory_)
 : _directory(std::move(directory_)) {
 open_watch();
}

DirectoryWatcher::DirectoryWatcher(DirectoryWatcher const& other_)
 : _directory(other_._directory) {
 open_watch();
}

DirectoryWatcher::DirectoryWatcher(DirectoryWatcher&& other_) noexcept
 : _directory(std::move(other_._directory))
 , _fd(std::exchange(other_._fd, -1))
 , _watch_descriptor(std::exchange(other_._watch_descriptor, -1)) {
}

auto DirectoryWatcher::operator=(DirectoryWatcher const& other_) -> DirectoryWatcher& {
 if (this == &other_) {
  return *this;
 }

 DirectoryWatcher temp(other_);
 return *this = std::move(temp);
}

auto DirectoryWatcher::operator=(DirectoryWatcher&& other_) noexcept -> DirectoryWatcher& {
 if (this == &other_) {
  return *this;
 }

 close_watch();
 _directory = std::move(other_._directory);
 _fd = std::exchange(other_._fd, -1);
 _watch_descriptor = std::exchange(other_._watch_descriptor, -1);
 return *this;
}

DirectoryWatcher::~DirectoryWatcher() {
 close_watch();
}

auto DirectoryWatcher::get_directory() const -> std::filesystem::path const& {
 return _directory;
}

auto DirectoryWatcher::wait_for_event() -> Notification {
 alignas(inotify_event) std::array<char, EVENT_BUFFER_SIZE> buffer{};

 while (true) {
  ssize_t const len = ::read(_fd, buffer.data(), buffer.size());
  if (len < 0) {
   if (errno == EINTR) {
    continue;
   }
   throw std::system_error(errno, std::system_category(), "read");
  }

  if (len == 0) {
   continue;
  }

  auto* ptr = buffer.data();
  auto* const end = buffer.data() + len;
  while (ptr < end) {
   auto const* event = reinterpret_cast<inotify_event const*>(ptr);
   auto const consumed = sizeof(inotify_event) + event->len;
   ptr += consumed;

   if ((event->mask & IN_ISDIR) != 0) {
    continue;
   }

   if (event->len == 0 || event->name[0] == '\0') {
    continue;
   }

   Notification notification;
   if (event->mask & IN_DELETE) {
    notification._event = Event::Deleted;
   } else if (event->mask & IN_MODIFY) {
    notification._event = Event::Modified;
   } else if (event->mask & IN_CREATE) {
    notification._event = Event::Created;
   } else {
    continue;
   }

   notification._path = _directory / std::string(event->name);
   return notification;
  }
 }
}

void DirectoryWatcher::open_watch() {
 if (_directory.empty()) {
  throw std::runtime_error("DirectoryWatcher: directory path is empty");
 }

 close_watch();

 _fd = ::inotify_init1(IN_CLOEXEC);
 if (_fd == -1) {
  throw std::system_error(errno, std::system_category(), "inotify_init1");
 }

 auto dir_native = _directory.string();
 _watch_descriptor = ::inotify_add_watch(_fd, dir_native.c_str(), WATCH_MASK);
 if (_watch_descriptor == -1) {
  auto const err = errno;
  ::close(_fd);
  _fd = -1;
  throw std::system_error(err, std::system_category(), "inotify_add_watch");
 }
}

void DirectoryWatcher::close_watch() {
 if (_fd == -1) {
  return;
 }

 if (_watch_descriptor != -1) {
  ::inotify_rm_watch(_fd, _watch_descriptor);
  _watch_descriptor = -1;
 }

 ::close(_fd);
 _fd = -1;
}

}  // namespace pmt::util
