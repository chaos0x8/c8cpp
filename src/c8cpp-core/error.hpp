#pragma once

#include <stdexcept>
#include <string_view>

// FILE IS GENERATED

namespace C8::Core::Error {
  class Error : public std::exception {
  public:
    Error(std::string_view);

    const char* what() const throw() override
    {
      return what_.c_str();
    }

  private:
    std::string what_;
  };

  class AssertError : public Error {
  public:
    AssertError(std::string_view);
  };

  class StandardError : public Error {
  public:
    StandardError(std::string_view);
  };

  class ArgumentError : public Error {
  public:
    ArgumentError(std::string_view);
  };

} // namespace C8::Core::Error
