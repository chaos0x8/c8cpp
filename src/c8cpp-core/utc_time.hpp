#pragma once

#include <chrono>
#include <filesystem>

namespace C8::Core {
  class UtcTime {
  public:
    explicit UtcTime(std::chrono::utc_clock::time_point);
    explicit UtcTime(std::chrono::system_clock::time_point);
    explicit UtcTime(std::string);

    static UtcTime now();
    static UtcTime mtime(const std::filesystem::path&);

    std::string string() const;
    std::string format(std::string_view formatString) const;

    UtcTime operator+(std::chrono::seconds) const;
    UtcTime operator-(std::chrono::seconds) const;

    auto operator<=>(const UtcTime&) const = default;

  private:
    std::chrono::utc_clock::time_point tp_;
  };
}; // namespace C8::Core
