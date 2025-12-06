#pragma once

#include <filesystem>

namespace pmt::util {

class DirectoryWatcher {
public:
 // -$ Lifetime $--
 explicit DirectoryWatcher(std::filesystem::path directory_);
 DirectoryWatcher(DirectoryWatcher const& other_);
 DirectoryWatcher(DirectoryWatcher&& other_) noexcept;
 auto operator=(DirectoryWatcher const& other_) -> DirectoryWatcher&;
 auto operator=(DirectoryWatcher&& other_) noexcept -> DirectoryWatcher&;
 ~DirectoryWatcher();

 // -$ Observers $--
 [[nodiscard]] auto get_directory() const -> std::filesystem::path const&;

 enum class Event {
  Created,
  Modified,
  Deleted,
 };

 struct Notification {
  Event _event = Event::Created;
  std::filesystem::path _path;
 };

 // -$ Actions $--
 [[nodiscard]] auto wait_for_event() -> Notification;

private:
 // -$ Data $--
 std::filesystem::path _directory;
 int _fd = -1;
 int _watch_descriptor = -1;

 // -$ Helpers $--
 void open_watch();
 void close_watch();
};

}  // namespace pmt::util
