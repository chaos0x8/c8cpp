#pragma once

#include <filesystem>
#include <format>
#include <stdexcept>
#include <string>
#include <string_view>

namespace C8::Cipher::Error {
  class Error : public std::exception {
  public:
    Error(std::string_view message) : message_(message) {}

    const char* what() const throw() override
    {
      return message_.c_str();
    }

  private:
    std::string message_;
  };

  class AssertError : public Error {
  public:
    AssertError(std::string_view);
  };

  class StandardError : public Error {
  public:
    StandardError(std::string_view);
  };

  class FileError : public Error {
  public:
    FileError(std::string_view);

    static FileError readError(const std::filesystem::path&);
  };

  class ConnectionRefusedError : public Error {
  public:
    ConnectionRefusedError(std::string_view);

    static ConnectionRefusedError acceptError();
    static ConnectionRefusedError connectError();
  };

  template <typename... Args>
  inline void checkSuccess(bool condition, std::string_view failureMessage, Args&&... args)
  {
    if (!condition) {
      if constexpr (sizeof...(Args) > 0) {
        throw StandardError(std::format(failureMessage, std::forward<Args>(args)...));
      } else {
        throw StandardError(failureMessage);
      }
    }
  }
} // namespace C8::Cipher::Error
