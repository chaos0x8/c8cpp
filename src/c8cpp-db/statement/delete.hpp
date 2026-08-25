#pragma once

#include "c8cpp-db/detail/meta.hpp"
#include "c8cpp-db/statement/common.hpp"
#include "c8cpp-db/statement/query.hpp"
#include "c8cpp-db/statement/statement.hpp"

namespace C8::Db::Statement {
  template <typename TableDef>
  struct Delete : public Statement {
    std::string string() const override
    {
      std::string_view tableName = std::meta::identifier_of(^^TableDef);

      if (where_.empty()) {
        return std::format("DELETE FROM {};", tableName);
      }

      return std::format("DELETE FROM {} WHERE {};", tableName, where_.string());
    }

    Params params() const override
    {
      return where_.params();
    }

    Query& where()
    {
      return where_;
    }

  private:
    Query where_;
  };
} // namespace C8::Db::Statement
