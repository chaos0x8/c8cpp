#include "c8cpp-core/utc_time.hpp"
#include "c8cpp-core/error.hpp"

#include <sstream>

namespace C8::Core {
  UtcTime::UtcTime(std::chrono::utc_clock::time_point tp) : tp_(tp) {}

  UtcTime::UtcTime(std::chrono::system_clock::time_point tp) : tp_(std::chrono::utc_clock::from_sys(tp)) {}

  UtcTime::UtcTime(std::string input) {
    std::istringstream ss(std::move(input));
    std::chrono::from_stream(ss, "%F %T", tp_);

    if (ss.fail()) {
      throw Error::ArgumentError("Cannot convert string to UtcTime.");
    }
  }

  UtcTime UtcTime::now() {
    return UtcTime(std::chrono::utc_clock::now());
  }

  UtcTime UtcTime::mtime(const std::filesystem::path& path) {
    return UtcTime(std::chrono::clock_cast<std::chrono::utc_clock>(std::filesystem::last_write_time(path)));
  }

  std::string UtcTime::string() const {
    return std::format("{0:%F} {0:%T}", tp_);
  }

  std::string UtcTime::format(std::string_view formatString) const {
    return std::vformat(formatString, std::make_format_args(tp_));
  }

  UtcTime UtcTime::operator+(std::chrono::seconds delta) const {
    return UtcTime(tp_ + delta);
  }

  UtcTime UtcTime::operator-(std::chrono::seconds delta) const {
    return UtcTime(tp_ - delta);
  }
} // namespace C8::Core
