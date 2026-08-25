#pragma once

#include "c8cpp-db/detail/sqlite3_statement.hpp"
#include "c8cpp-db/error.hpp"
#include "c8cpp-db/statement.hpp"

#include <expected>
#include <filesystem>
#include <generator>
#include <iostream>
#include <memory>
#include <sqlite3.h>
#include <sstream>
#include <string_view>
#include <variant>
#include <vector>

namespace C8::Db {
  namespace Detail {
    class SqLite3DbDeleter {
    public:
      void operator()(sqlite3* ptr) const;
    };

    using SqLite3DbPointer = std::unique_ptr<sqlite3, SqLite3DbDeleter>;
  } // namespace Detail

  struct RowNotFound_t {
  };

  static constexpr RowNotFound_t rowNotFound{};

  class SqLite3Wrapper {
  public:
    SqLite3Wrapper(std::filesystem::path path);

    void setInMomeryJournal();
    std::filesystem::path path() const;

  private:
    static Detail::SqLite3DbPointer openDb(std::filesystem::path path);

    std::filesystem::path path_;
    Detail::SqLite3DbPointer db_ = openDb(path_);

  private:
    template <typename TableDef>
    void fill(TableDef& row, const Detail::SqLite3Statement& statement) const
    {
      size_t idx = 1;

      template for (constexpr auto member : std::define_static_array(
                      std::meta::nonstatic_data_members_of(^^TableDef, std::meta::access_context::current())))
      {
        using U = typename[:std::meta::type_of(member):];
        Column<U> col;

        if constexpr (std::meta::identifier_of(member) == "rowid") {
          col.loadFrom_(statement, 0);
        } else {
          col.loadFrom_(statement, idx++);
        }

        row.[:member:] = std::move(col.value);
      }
    }

    template <typename TableDef>
    std::generator<TableDef> selectImpl(std::string query, const Statement::Params params)
    {
      auto statement = Detail::prepareStatement(*db_, query);

      for (size_t i = 0; i < params.size(); ++i) {
        std::visit([&statement, i](const auto& val) { statement.bind(val, i + 1); }, params[i]);
      }

      while (true) {
        const int err = sqlite3_step(statement.get());

        if (err == SQLITE_DONE) {
          break;
        }

        if (err == SQLITE_ROW) {
          TableDef row;
          fill(row, statement);
          co_yield std::move(row);
        } else {
          throw Error::SqLite3Error(std::format("Execute error, error code = {}", err), *db_.get());
        }
      }
    }

    void executeImpl(std::string_view query, Statement::Params params)
    {
      auto statement = Detail::prepareStatement(*db_, query);

      for (size_t i = 0; i < params.size(); ++i) {
        std::visit([&statement, i](const auto& val) { statement.bind(val, i + 1); }, params[i]);
      }

      while (true) {
        const int err = sqlite3_step(statement.get());

        if (err == SQLITE_DONE) {
          break;
        }

        if (err != SQLITE_ROW) {
          throw Error::SqLite3Error(std::format("Execute error, error code = {}", err), *db_.get());
        }
      }
    }

  private:
    template <typename... ColumnDef, size_t... Indexes>
    void insertImpl(std::string_view query, const std::tuple<ColumnDef...>& args, std::index_sequence<Indexes...>)
    {
      Statement::Params params;
      (params(std::get<Indexes>(args).getDbValue()), ...);
      executeImpl(query, std::move(params));
    }

  public:
    template <typename StatementType>
    void exec(StatementType stm)
    {
      executeImpl(stm.string(), stm.params());
    }

    template <typename TableDef>
    int64_t exec(Statement::Insert<TableDef> stm)
    {
      executeImpl(stm.string(), stm.params());
      return sqlite3_last_insert_rowid(db_.get());
    }

    template <typename TableDef>
    std::generator<TableDef> exec(Statement::Select<TableDef> stm)
    {
      return selectImpl<TableDef>(stm.string(), stm.params());
    }

    template <typename TableDef>
    int64_t insert(const TableDef& record)
    {
      Statement::Insert<TableDef> stm;
      stm.values() | record;
      return exec(stm);
    }

    template <typename TableDef>
    std::expected<TableDef, RowNotFound_t> find(std::nothrow_t, Statement::Select<TableDef> stm)
    {
      for (auto row : exec(std::move(stm))) {
        return row;
      }

      return std::unexpected(rowNotFound);
    }

    template <typename TableDef>
    TableDef find(Statement::Select<TableDef> stm)
    {
      return find(std::nothrow, std::move(stm))
        .or_else([](RowNotFound_t) -> std::expected<TableDef, RowNotFound_t> { throw Error::RowNotFoundError(); })
        .value();
    }

    template <typename TableDef>
    std::vector<TableDef> collect(Statement::Select<TableDef> stm)
    {
      std::vector<TableDef> result;
      for (auto row : exec(stm)) {
        result.emplace_back(std::move(row));
      }
      return result;
    }

    template <typename TableDef>
    size_t count(Statement::Select<TableDef> stm)
    {
      size_t result{0};
      for (auto row : exec(std::move(stm))) {
        ++result;
      }
      return result;
    }
  };
} // namespace C8::Db
