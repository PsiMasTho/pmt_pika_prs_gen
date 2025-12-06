#pragma once

namespace pmt::util::test {

class DirectoryWatcherTest {
public:
 static void run();

private:
 static void test_wait_for_create();
 static void test_wait_for_modify();
 static void test_wait_for_delete();
 static void test_copyable_watcher();
};

}  // namespace pmt::util::test
