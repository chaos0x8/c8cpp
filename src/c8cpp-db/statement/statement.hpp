#pragma once

#include "c8cpp-db/column_definition.hpp"
#include "c8cpp-db/statement/params.hpp"
#include "c8cpp-db/statement/query.hpp"

#include <meta>
#include <string>

namespace C8::Db::Statement {
  struct Statement {
    virtual ~Statement() = default;

    virtual std::string string() const = 0;
    virtual Params params() const = 0;

    bool valid() const;

    template <std::meta::info info>
    static consteval auto q()
    {
      return Q<info>();
    }
  };
} // namespace C8::Db::Statement
