#pragma once

#include <string_view>

namespace C8::Network {
  struct FileDescriptor {
    using value_type = int;

    static constexpr value_type InvalidValue = value_type{-1};

    FileDescriptor() = default;
    explicit FileDescriptor(value_type fd);
    FileDescriptor(const FileDescriptor&) = delete;
    FileDescriptor(FileDescriptor&& other);
    ~FileDescriptor();

    FileDescriptor& operator=(const FileDescriptor&) = delete;
    FileDescriptor& operator=(FileDescriptor&& other);

    explicit operator bool() const;

    value_type raw() const;

    void reset();

  private:
    value_type value_ = InvalidValue;
  };
} // namespace C8::Network
