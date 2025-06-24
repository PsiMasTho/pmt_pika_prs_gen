#include "pmt/util/timestamp.hpp"

#include <chrono>
#include <iomanip>
#include <sstream>

namespace pmt::util {
auto get_timestamp() -> std::string {
 auto const now = std::chrono::system_clock::now();
 auto const time = std::chrono::system_clock::to_time_t(now);
 auto const local_time = *std::localtime(&time);

 std::ostringstream oss;
 oss << std::put_time(&local_time, "%Y-%m-%d %H:%M:%S");

 return oss.str();
}

}  // namespace pmt::util