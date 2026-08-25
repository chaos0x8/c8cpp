#pragma once

#include <stdexcept>
#include <string>

namespace C8::Network::Error {
  class Error : public std::exception {
  public:
    explicit Error(std::string what);

    const char* what() const throw() override;

  private:
    std::string what_;
  };

  class SystemError : public Error {
  public:
    explicit SystemError(long err);
  };

  class IoError : public Error {
  public:
    explicit IoError(long err);

    long err() const
    {
      return err_;
    }

  private:
    long err_{};
  };
}; // namespace C8::Network::Error
