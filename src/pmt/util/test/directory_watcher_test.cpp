#include "pmt/util/test/directory_watcher_test.hpp"

#include "pmt/util/directory_watcher.hpp"

#include <cassert>
#include <chrono>
#include <filesystem>
#include <fstream>
#include <system_error>
#include <thread>

namespace pmt::util::test {
namespace {
using namespace std::chrono_literals;

struct TempDir {
 TempDir()
  : _path(make_path()) {
 }
 ~TempDir() {
  std::error_code ec;
  std::filesystem::remove_all(_path, ec);
 }

 auto path() const -> std::filesystem::path const& {
  return _path;
 }

 static auto make_path() -> std::filesystem::path {
  auto const base = std::filesystem::temp_directory_path();
  auto const dir = base / std::filesystem::path("pmt_directory_watcher_test");
  std::filesystem::create_directories(dir);
  return dir;
 }

 std::filesystem::path _path;
};

void write_file(std::filesystem::path const& path_) {
 std::filesystem::create_directories(path_.parent_path());

 std::ofstream out(path_, std::ios::binary | std::ios::trunc);
 out << "data";
 out.flush();
}
}  // namespace

void DirectoryWatcherTest::run() {
 test_wait_for_create();
 test_wait_for_modify();
 test_wait_for_delete();
 test_copyable_watcher();
}

void DirectoryWatcherTest::test_wait_for_create() {
 TempDir watch_dir;
 DirectoryWatcher watcher(watch_dir.path());
 auto const target = watch_dir.path() / "created.txt";

 std::thread writer([target]() {
  std::this_thread::sleep_for(50ms);
  write_file(target);
 });

 auto const notification = watcher.wait_for_event();
 writer.join();
 assert(notification._event == DirectoryWatcher::Event::Created);
 assert(notification._path == target);
}

void DirectoryWatcherTest::test_wait_for_modify() {
 TempDir watch_dir;
 auto const target = watch_dir.path() / "modified.txt";
 write_file(target);

 DirectoryWatcher watcher(watch_dir.path());

 std::thread modifier([target]() {
  std::this_thread::sleep_for(50ms);
  std::ofstream out(target, std::ios::binary | std::ios::app);
  out << "more";
 });

 auto const notification = watcher.wait_for_event();
 modifier.join();
 assert(notification._event == DirectoryWatcher::Event::Modified);
 assert(notification._path == target);
}

void DirectoryWatcherTest::test_wait_for_delete() {
 TempDir watch_dir;
 auto const target = watch_dir.path() / "deleted.txt";
 write_file(target);

 DirectoryWatcher watcher(watch_dir.path());

 std::thread remover([target]() {
  std::this_thread::sleep_for(50ms);
  std::filesystem::remove(target);
 });

 auto const notification = watcher.wait_for_event();
 remover.join();
 assert(notification._event == DirectoryWatcher::Event::Deleted);
 assert(notification._path == target);
}

void DirectoryWatcherTest::test_copyable_watcher() {
 TempDir watch_dir;
 DirectoryWatcher watcher(watch_dir.path());
 DirectoryWatcher watcher_copy = watcher;
 auto const target = watch_dir.path() / "shared.txt";

 DirectoryWatcher::Notification observed_primary;
 DirectoryWatcher::Notification observed_copy;

 std::thread wait_primary([&]() { observed_primary = watcher.wait_for_event(); });
 std::thread wait_copy([&]() { observed_copy = watcher_copy.wait_for_event(); });

 std::thread writer([target]() {
  std::this_thread::sleep_for(50ms);
  write_file(target);
 });

 writer.join();
 wait_primary.join();
 wait_copy.join();

 assert(observed_primary._event == DirectoryWatcher::Event::Created);
 assert(observed_copy._event == DirectoryWatcher::Event::Created);
 assert(observed_primary._path == target);
 assert(observed_copy._path == target);
}

}  // namespace pmt::util::test
