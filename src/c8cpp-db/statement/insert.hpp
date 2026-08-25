#pragma once

#include "c8cpp-db/statement/common.hpp"
#include "c8cpp-db/statement/statement.hpp"

namespace C8::Db::Statement {
  template <typename TableDef>
  struct Insert : public Statement {
    std::string string() const override
    {
      std::string_view tableName = std::meta::identifier_of(^^TableDef);

      const std::string columns = join(values_.columns(), ",");
      const std::string values = join(transform(values_.columns(), "?"), ",");

      return std::format("INSERT INTO {}({}) VALUES({});", tableName, columns, values);
    }

    Params params() const override
    {
      return values_.params();
    }

    Query& values()
    {
      return values_;
    }

  private:
    Query values_;
  };
} // namespace C8::Db::Statement
