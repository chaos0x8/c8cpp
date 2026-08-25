#include "c8cpp-db/statement/statement.hpp"

#include <algorithm>

namespace C8::Db::Statement {
  bool Statement::valid() const {
    const std::string query = string();
    const size_t requiredParams = std::count(query.begin(), query.end(), '?');
    return requiredParams == params().size();
  }
} // namespace C8::Db::Statement
