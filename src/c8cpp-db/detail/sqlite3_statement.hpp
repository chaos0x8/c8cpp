#pragma once

#include "c8cpp-db/error.hpp"

#include <format>
#include <memory>
#include <meta>
#include <sqlite3.h>

namespace C8::Db::Detail {
  class SqLite3StatementDeleter {
  public:
    void operator()(sqlite3_stmt* ptr) const;
  };

  class SqLite3Statement {
  public:
    SqLite3Statement(sqlite3_stmt* statement);

    void bind(std::string_view val, size_t index);
    void bind(double val, size_t index);
    void bind(int32_t val, size_t index);
    void bind(int64_t val, size_t index);

    int columnCount() const;
    int columnType(int index) const;
    std::string columnName(int index) const;

    std::string columnText(int index) const;
    int32_t columnInt32(int index) const;
    int64_t columnInt64(int index) const;
    double columnDouble(int index) const;

    sqlite3_stmt* get();

    explicit operator bool() const;

  private:
    void checkBindResult(const int err, size_t index) const;

    std::unique_ptr<sqlite3_stmt, SqLite3StatementDeleter> statement_;
  };

  SqLite3Statement prepareStatement(sqlite3& db, std::string_view query);
} // namespace C8::Db::Detail
