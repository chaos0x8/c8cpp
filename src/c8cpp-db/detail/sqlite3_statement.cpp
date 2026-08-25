#include "c8cpp-db/detail/sqlite3_statement.hpp"

namespace C8::Db::Detail {
  void SqLite3StatementDeleter::operator()(sqlite3_stmt* ptr) const
  {
    if (ptr) {
      sqlite3_finalize(ptr);
    }
  }

  SqLite3Statement::SqLite3Statement(sqlite3_stmt* statement) : statement_(statement) {}

  void SqLite3Statement::bind(std::string_view val, size_t index)
  {
    checkBindResult(sqlite3_bind_text(statement_.get(), index, val.data(), val.size(), nullptr), index);
  }

  void SqLite3Statement::bind(double val, size_t index)
  {
    checkBindResult(sqlite3_bind_double(statement_.get(), index, val), index);
  }

  void SqLite3Statement::bind(int32_t val, size_t index)
  {
    checkBindResult(sqlite3_bind_int(statement_.get(), index, val), index);
  }

  void SqLite3Statement::bind(int64_t val, size_t index)
  {
    checkBindResult(sqlite3_bind_int64(statement_.get(), index, val), index);
  }

  int SqLite3Statement::columnCount() const
  {
    const int numCols = sqlite3_column_count(statement_.get());
    if (numCols < 0) {
      throw Error::StandardError("Failed to get statement column count.");
    }
    return numCols;
  }

  int SqLite3Statement::columnType(int index) const
  {
    return sqlite3_column_type(statement_.get(), index);
  }

  std::string SqLite3Statement::columnName(int index) const
  {
    return sqlite3_column_name(statement_.get(), index);
  }

  std::string SqLite3Statement::columnText(int index) const
  {
    const auto size = sqlite3_column_bytes(statement_.get(), index);
    const auto data = sqlite3_column_text(statement_.get(), index);
    std::string result;
    result.append(reinterpret_cast<const char*>(data), size);
    return result;
  }

  int32_t SqLite3Statement::columnInt32(int index) const
  {
    return sqlite3_column_int(statement_.get(), index);
  }

  int64_t SqLite3Statement::columnInt64(int index) const
  {
    return sqlite3_column_int64(statement_.get(), index);
  }

  double SqLite3Statement::columnDouble(int index) const
  {
    return sqlite3_column_double(statement_.get(), index);
  }

  sqlite3_stmt* SqLite3Statement::get()
  {
    return statement_.get();
  }

  SqLite3Statement::operator bool() const
  {
    return static_cast<bool>(statement_);
  }

  void SqLite3Statement::checkBindResult(const int err, size_t index) const
  {
    if (err != SQLITE_OK) {
      throw Error::StandardError(
        std::format("Failed to bind parameter to query, index = {}, error code = {}", index, err));
    }
  }

  SqLite3Statement prepareStatement(sqlite3& db, std::string_view query)
  {
    sqlite3_stmt* statement = nullptr;
    const int err = sqlite3_prepare_v2(&db, query.data(), query.size(), &statement, nullptr);
    SqLite3Statement result(statement);
    if (!result || err != SQLITE_OK) {
      throw Error::SqLite3Error(std::format("Cannot prepare statement `{}', error code = {}", query, err), db);
    }
    return result;
  }
} // namespace C8::Db::Detail
