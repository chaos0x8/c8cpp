#pragma once

#include "c8cpp-db/detail/meta.hpp"
#include <sstream>

namespace C8::Db::TestUtility {
  namespace Detail {
    template <typename T>
    inline constexpr std::weak_ordering compareStruct(const T& left, const T& right)
    {
      template for (constexpr auto member :
        std::define_static_array(std::meta::nonstatic_data_members_of(^^T, std::meta::access_context::current())))
      {
        if constexpr (std::meta::identifier_of(member) == "rowid") {
          continue;
        } else {
          const auto r = left.[:member:] <=> right.[:member:];

          if (r == std::weak_ordering::equivalent) {
            continue;
          } else if (r == std::strong_ordering::less) {
            return std::weak_ordering::less;
          } else if (r == std::strong_ordering::greater) {
            return std::weak_ordering::greater;
          }
        }

        std::unreachable();
      }

      return std::weak_ordering::equivalent;
    }

    template <typename T>
    concept Collection = requires(T t)
    {
      t.begin();
      t.end();
    };

    template <typename T>
    inline constexpr std::string structToString(const T& o)
    {
      std::stringstream ss;

      size_t idx = 0;

      ss << std::meta::identifier_of(std::meta::dealias(^^T)) << "{ ";
      template for (constexpr auto member :
        std::define_static_array(std::meta::nonstatic_data_members_of(^^T, std::meta::access_context::current())))
      {
        if (idx++ != 0) {
          ss << ", ";
        }

        using U = typename[:std::meta::type_of(member):];
        ss << std::meta::identifier_of(member) << " = ";
        if constexpr (Collection<U>) {
          ss << "[ ";
          for (auto& element : o.[:member:]) {
            size_t elem_idx = 0;

            if (elem_idx++ != 0) {
              ss << ", ";
            }

            if constexpr (std::meta::is_enum_type(std::meta::decay(std::meta::type_of(std::meta::dealias(^^element)))))
            {
              ss << C8::Core::enumToString(element);
            } else {
              ss << element;
            }
          }
          ss << " ]";
        } else if constexpr (std::meta::is_enum_type(std::meta::type_of(std::meta::dealias(member)))) {
          ss << C8::Core::enumToString(o.[:member:]);
        } else {
          ss << o.[:member:];
        }
      }
      ss << " }";

      return ss.str();
    }
  } // namespace Detail

  enum class EnumString : int32_t { Value1 = 1, Value2 };

  struct TestTable {
    bool operator==(const TestTable& other) const
    {
      return Detail::compareStruct(*this, other) == std::weak_ordering::equivalent;
    }

    inline friend std::ostream& operator<<(std::ostream& os, const TestTable& table)
    {
      return os << Detail::structToString(table);
    }

    std::string Key;
    std::string Value;
    int64_t rowid;
  };

  struct TestTableEx {
    bool operator==(const TestTableEx& other) const
    {
      return Detail::compareStruct(*this, other) == std::weak_ordering::equivalent;
    }

    inline friend std::ostream& operator<<(std::ostream& os, const TestTableEx& table)
    {
      return os << Detail::structToString(table);
    }

    std::string Text;
    int32_t LowNumber;
    int64_t HighNumber;
    double DoubleNumber;
    std::vector<int64_t> Numbers;
    std::vector<EnumString> Enums;
    int64_t rowid;
  };

  struct TestTable2 {
    bool operator==(const TestTable2& other) const
    {
      return Detail::compareStruct(*this, other) == std::weak_ordering::equivalent;
    }

    inline friend std::ostream& operator<<(std::ostream& os, const TestTable2& table)
    {
      return os << Detail::structToString(table);
    }

    std::string Key;
    int64_t rowid;
  };

  struct TestTable3 {
    bool operator==(const TestTable3& other) const
    {
      return Detail::compareStruct(*this, other) == std::weak_ordering::equivalent;
    }

    inline friend std::ostream& operator<<(std::ostream& os, const TestTable3& table)
    {
      return os << Detail::structToString(table);
    }

    EnumString Value;
    int64_t rowid;
  };
} // namespace C8::Db::TestUtility
