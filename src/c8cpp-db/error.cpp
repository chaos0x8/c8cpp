#include "c8cpp-db/error.hpp"

#include <format>

namespace C8::Db::Error {
  Error::Error(const std::string& message) : std::runtime_error(message) {}

  StandardError::StandardError(std::string_view message) : Error(std::format("Db::StandardError: {}", message)) {}

  RowNotFoundError::RowNotFoundError() : Error("Db::RowNotFound") {}

  SqLite3Error::SqLite3Error(std::string_view message, sqlite3& db)
    : Error(std::format("Db::SqLite3Error: {}, reason: \"{}\"", message, sqlite3_errmsg(&db))) {}
} // namespace C8::Db::Error
