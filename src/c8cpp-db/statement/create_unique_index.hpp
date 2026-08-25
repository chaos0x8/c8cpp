#pragma once

#include "c8cpp-db/detail/meta.hpp"
#include "c8cpp-db/statement/common.hpp"
#include "c8cpp-db/statement/query.hpp"
#include "c8cpp-db/statement/statement.hpp"

namespace C8::Db::Statement {
  template <typename TableDef>
  struct CreateUniqueIndex : public Statement {
    void ifNotExists()
    {
      ifNotExists_ = true;
    }

    std::string string() const override
    {
      std::string_view tableName = std::meta::identifier_of(^^TableDef);

      if (ifNotExists_) {
        return std::format(
          "CREATE UNIQUE INDEX IF NOT EXISTS {}_Index ON {} ({})", tableName, tableName, index_.string());
      }

      return std::format("CREATE UNIQUE INDEX {}_Index ON {} ({})", tableName, tableName, index_.string());
    }

    Params params() const override
    {
      return {};
    }

    Query& index()
    {
      return index_;
    }

  private:
    bool ifNotExists_ = false;
    Query index_;
  };
} // namespace C8::Db::Statement
