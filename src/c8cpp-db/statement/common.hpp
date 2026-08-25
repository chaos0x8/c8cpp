#pragma once

#include <string>
#include <vector>

namespace C8::Db::Statement {
  inline std::string join(const std::vector<std::string>& collection, const std::string& separator)
  {
    std::string result;
    for (size_t i = 0; i < collection.size(); ++i) {
      if (i > 0) {
        result += ",";
      }
      result += collection[i];
    }
    return result;
  }

  inline std::vector<std::string> transform(const std::vector<std::string>& collection, const std::string& val)
  {
    std::vector<std::string> out;
    for (size_t i = 0; i < collection.size(); ++i) {
      out.emplace_back(val);
    }
    return out;
  }
} // namespace C8::Db::Statement
