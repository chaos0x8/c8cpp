#pragma once

#include "c8cpp-db/column_definition.hpp"
#include "c8cpp-db/error.hpp"
#include "c8cpp-db/statement/common.hpp"
#include "c8cpp-db/statement/params.hpp"

#include <format>
#include <meta>
#include <optional>

namespace C8::Db::Statement {
  enum class Operator { Nil = 0, Assign, Equal, Greater };

  struct Col {
    std::string_view value;
  };

  template <std::meta::info Col>
  struct Q {
    using ColType = typename[:std::meta::type_of(Col):];

    template <typename T>
    Q& operator=(T value)
    {
      operator_ = Operator::Assign;
      setValue(std::move(value));
      return *this;
    }

    template <typename T>
    Q& operator>(T value)
    {
      operator_ = Operator::Greater;
      setValue(std::move(value));
      return *this;
    }

    template <typename T>
    Q& operator==(T value)
    {
      operator_ = Operator::Equal;
      setValue(std::move(value));
      return *this;
    }

    template <typename T>
    void setValue(T value)
    {
      col_.emplace();

      col_->setNativeValue(std::move(value));
    }

    auto getDbValue() const
    {
      if (!col_) {
        throw Error::StandardError("Uninitialized.");
      }

      return col_->getDbValue();
    }

    bool hasValue() const
    {
      return static_cast<bool>(col_);
    }

    std::string string() const
    {
      if (operator_ == Operator::Nil) {
        return std::string(columnName());
      }

      return std::format("{} {} ?", columnName(), operatorName());
    }

    std::string_view columnName() const
    {
      return std::define_static_string(std::meta::identifier_of(Col));
    }

    std::string operatorName() const
    {
      switch (operator_) {
        case Operator::Assign:
          return "=";
        case Operator::Equal:
          return "==";
        case Operator::Greater:
          return ">";
        default:
          break;
      }

      throw Error::StandardError("Invalid operator.");
    }

    Operator operator_ = Operator::Nil;
    std::optional<Column<ColType>> col_;
  };

  struct Query {
    Query() = default;

    template <std::meta::info info>
    Query(const Q<info>& q)
    {
      query_ = q.string();
      columns_.emplace_back(q.columnName());
      if (q.hasValue()) {
        params_(q.getDbValue());
      }
    }

    template <std::meta::info info>
    Query& operator|(const Q<info>& q)
    {
      if (!query_.empty()) {
        query_ += ",";
      }
      query_ += q.string();
      columns_.emplace_back(q.columnName());
      if (q.hasValue()) {
        params_(q.getDbValue());
      }
      return *this;
    }

    template <typename TableDef>
    Query& operator|(const TableDef& row)
    {
      template for (constexpr auto member : std::define_static_array(
                      std::meta::nonstatic_data_members_of(^^TableDef, std::meta::access_context::current())))
      {
        if (std::meta::identifier_of(member) == "rowid") {
          continue;
        }

        (*this) | (Q<member>{} = row.[:member:]);
      }
      return *this;
    }

    template <std::meta::info info>
    Query& operator&&(const Q<info>& q)
    {
      if (!query_.empty()) {
        query_ += " AND ";
      }
      query_ += q.string();
      columns_.emplace_back(q.columnName());
      if (q.hasValue()) {
        params_(q.getDbValue());
      }
      return *this;
    }

    const std::string& string() const
    {
      return query_;
    }

    const std::vector<std::string>& columns() const
    {
      return columns_;
    }

    const Params& params() const
    {
      return params_;
    }

    bool empty() const
    {
      return query_.empty();
    }

  private:
    std::string query_;
    std::vector<std::string> columns_;
    Params params_;
  };
} // namespace C8::Db::Statement
