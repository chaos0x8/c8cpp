#pragma once

#include "c8cpp-db/detail/meta.hpp"
#include "c8cpp-db/statement/common.hpp"
#include "c8cpp-db/statement/statement.hpp"

namespace C8::Db::Statement {
  template <typename TableDef>
  struct CreateTable : public Statement {
    void ifNotExists()
    {
      ifNotExists_ = true;
    }

    static consteval std::string_view getColumns()
    {
      std::string result;

      template for (constexpr auto member : std::define_static_array(
                      std::meta::nonstatic_data_members_of(^^TableDef, std::meta::access_context::current())))
      {
        if (std::meta::identifier_of(member) == "rowid") {
          continue;
        }

        if (!result.empty()) {
          result.append(", ");
        }

        using U = typename[:std::meta::type_of(member):];
        result.append(
          std::format("{} {}", std::meta::identifier_of(member), Detail::dbTypeName<typename Column<U>::DbT>()));
      }

      return std::define_static_string(result);
    }

    std::string string() const override
    {
      std::string_view tableName = std::meta::identifier_of(^^TableDef);
      std::string_view columns = getColumns();

      if (ifNotExists_) {
        return std::format("CREATE TABLE IF NOT EXISTS {}({});", tableName, columns);
      }

      return std::format("CREATE TABLE {}({});", tableName, columns);
    }

    Params params() const override
    {
      return {};
    }

  private:
    bool ifNotExists_ = false;
  };
} // namespace C8::Db::Statement
