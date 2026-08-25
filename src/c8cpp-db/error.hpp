#pragma once

#include <stdexcept>
#include <string>
#include <string_view>

#include <sqlite3.h>

namespace C8::Db::Error {
  class Error : public std::runtime_error {
  public:
    Error(const std::string& message);
  };

  class StandardError : public Error {
  public:
    StandardError(std::string_view message);
  };

  class RowNotFoundError : public Error {
  public:
    RowNotFoundError();
  };

  class SqLite3Error : public Error {
  public:
    SqLite3Error(std::string_view message, sqlite3&);
  };
} // namespace C8::Db::Error
