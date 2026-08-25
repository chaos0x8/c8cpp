#include "c8cpp-network/file_descriptor.hpp"

#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <utility>

namespace C8::Network {
  FileDescriptor::FileDescriptor(value_type value) : value_(value) {}

  FileDescriptor::FileDescriptor(FileDescriptor&& other) : value_(std::exchange(other.value_, InvalidValue)) {}

  FileDescriptor& FileDescriptor::operator=(FileDescriptor&& other) {
    reset();
    std::swap(other.value_, value_);
    return *this;
  }

  FileDescriptor::~FileDescriptor() {
    reset();
  }

  FileDescriptor::operator bool() const {
    return value_ != InvalidValue;
  }

  FileDescriptor::value_type FileDescriptor::raw() const {
    return value_;
  }

  void FileDescriptor::reset() {
    if (operator bool()) {
      ::close(value_);
    }
    value_ = InvalidValue;
  }
} // namespace C8::Network
