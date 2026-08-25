#pragma once

#include "c8cpp-db/detail/meta.hpp"
#include "c8cpp-db/detail/sqlite3_statement.hpp"
#include "c8cpp-db/utility/algorithm.hpp"

#include <format>
#include <meta>
#include <ostream>
#include <ranges>
#include <sstream>
#include <string>
#include <string_view>
#include <tuple>

namespace C8::Db {
  namespace Detail {
    template <typename T>
    concept EnumType = std::is_enum_v<T>;

    template <typename T>
    concept SignedNumber = std::is_same_v<T, int32_t> || std::is_same_v<T, int64_t>;

    template <typename T>
    concept EnumOrNumber = EnumType<T> || SignedNumber<T>;
  } // namespace Detail

  struct IfNotExists {
  };

  template <typename T>
  concept DbType = std::is_same_v<T, std::string> || std::is_same_v<T, int32_t> || std::is_same_v<T, int64_t> ||
                   std::is_same_v<T, double>;

  template <typename T>
  concept TypeMapper = requires(const typename T::NativeT& typeVal, const typename T::DbT& dbTypeVal)
  {
    requires DbType<typename T::DbT>;
    {
      T::fromDbType(dbTypeVal)
    }
    ->std::same_as<typename T::NativeT>;
    {
      T::toDbType(typeVal)
    }
    ->std::same_as<typename T::DbT>;
  };

  template <typename T>
  struct TypeTrait {
  };

  template <DbType T>
  struct TypeTrait<T> {
    using NativeT = T;
    using DbT = T;

    static NativeT fromDbType(const DbT& v)
    {
      return v;
    }

    static DbT toDbType(const NativeT& v)
    {
      return v;
    }
  };

  template <Detail::EnumType T>
  struct TypeTrait<T> {
    using NativeT = T;
    using DbT = std::underlying_type_t<T>;

    static NativeT fromDbType(const DbT& v)
    {
      return static_cast<NativeT>(v);
    }

    static DbT toDbType(const NativeT& v)
    {
      return std::to_underlying(v);
    }
  };

  template <Detail::EnumOrNumber T>
  struct TypeTrait<std::vector<T>> {
    using NativeT = std::vector<T>;
    using DbT = std::string;

    static NativeT fromDbType(const DbT& v)
    {
      using namespace std::string_view_literals;

      auto converter = [](auto view) {
        int64_t value = 0;
        std::from_chars(view.data(), view.data() + view.size(), value);
        return static_cast<T>(value);
      };

      return std::ranges::views::split(v, ","sv) | std::ranges::views::transform(converter) |
             std::ranges::to<std::vector>();
    }

    static DbT toDbType(const NativeT& v)
    {
      using namespace std::string_view_literals;

      auto toString = [](auto vv) { return std::to_string(static_cast<int64_t>(vv)); };

      return std::ranges::views::transform(v, toString) | std::ranges::views::join_with(","sv) |
             std::ranges::to<std::string>();
    }
  };

  template <TypeMapper T>
  struct TypeTrait<T> {
    using NativeT = typename T::NativeT;
    using DbT = typename T::DbT;

    static NativeT fromDbType(const DbT& v)
    {
      return T::fromDbType(v);
    }

    static DbT toDbType(const NativeT& v)
    {
      return T::toDbType(v);
    }
  };

  template <typename T>
  struct Column {
    using Trait = TypeTrait<T>;
    using NativeT = typename Trait::NativeT;
    using DbT = typename Trait::DbT;

    auto operator<=>(const Column&) const = default;

    void setNativeValue(NativeT v)
    {
      value = std::move(v);
    }

    void setDbValue(const DbT& v)
    {
      value = Trait::fromDbType(v);
    }

    NativeT value;

    DbT getDbValue() const
    {
      return Trait::toDbType(value);
    }

    void loadFrom_(const Detail::SqLite3Statement& statement, size_t index)
    {
      if constexpr (std::is_same_v<DbT, std::string>) {
        setDbValue(statement.columnText(static_cast<int>(index)));
      } else if constexpr (std::is_same_v<DbT, int32_t>) {
        setDbValue(statement.columnInt32(static_cast<int>(index)));
      } else if constexpr (std::is_same_v<DbT, int64_t>) {
        setDbValue(statement.columnInt64(static_cast<int>(index)));
      } else if constexpr (std::is_same_v<DbT, double>) {
        setDbValue(statement.columnDouble(static_cast<int>(index)));
      } else {
        static_assert(std::is_same_v<DbT, void>, "Type not supported.");
      }
    }
  };
} // namespace C8::Db
