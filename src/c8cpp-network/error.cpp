#include "c8cpp-network/error.hpp"

#include <cstring>
#include <format>

namespace C8::Network::Error {
  Error::Error(std::string what) : what_(std::move(what)) {}

  const char* Error::what() const throw()
  {
    return what_.c_str();
  }

  SystemError::SystemError(long err) : Error(std::format("Network::SystemError: {}", strerror(err))) {}

  IoError::IoError(long err) : Error(std::format("Network::IoError: {}", err)), err_(err) {}
} // namespace C8::Network::Error
