#pragma once

#include "c8cpp-db/statement/common.hpp"
#include "c8cpp-db/statement/query.hpp"
#include "c8cpp-db/statement/statement.hpp"

namespace C8::Db::Statement {
  template <typename TableDef>
  struct Update : public Statement {
    std::string string() const override
    {
      std::string_view tableName = std::meta::identifier_of(^^TableDef);

      if (where_.empty()) {
        return std::format("UPDATE {} SET {};", tableName, set_.string());
      }

      return std::format("UPDATE {} SET {} WHERE {};", tableName, set_.string(), where_.string());
    }

    Params params() const override
    {
      return set_.params() + where_.params();
    }

    Query& set()
    {
      return set_;
    }

    Query& where()
    {
      return where_;
    }

  private:
    Query set_;
    Query where_;
  };
} // namespace C8::Db::Statement
