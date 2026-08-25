#include "c8cpp-db/statement/insert.hpp"
#include "c8cpp-db/test_utility/test_table.hpp"

#include <gmock/gmock.h>

namespace C8::Db::Statement {
  using namespace TestUtility;
  using namespace testing;

  TEST(Test_Statement_Insert, producesInsertSqlCodeWithNoParams)
  {
    Insert<TestTable> stm;
    EXPECT_THAT(stm.string(), StrEq("INSERT INTO TestTable() VALUES();"));
    EXPECT_THAT(stm.params(), IsEmpty());
    EXPECT_THAT(stm.valid(), Eq(true));
  }

  TEST(Test_Statement_Insert, producesInsertSqlCode)
  {
    Insert<TestTable> stm;
    stm.values() | (stm.q<^^TestTable::Key>() = "key");
    EXPECT_THAT(stm.string(), StrEq("INSERT INTO TestTable(Key) VALUES(?);"));
    EXPECT_THAT(stm.params(), ElementsAre(Params::value_type("key")));
    EXPECT_THAT(stm.valid(), Eq(true));
  }

  TEST(Test_Statement_Insert, producesInsertSqlCodeMultiParams)
  {
    Insert<TestTable> stm;
    stm.values() | (stm.q<^^TestTable::Key>() = "key") | (stm.q<^^TestTable::Value>() = "val");
    EXPECT_THAT(stm.string(), StrEq("INSERT INTO TestTable(Key,Value) VALUES(?,?);"));
    EXPECT_THAT(stm.params(), ElementsAre(Params::value_type("key"), Params::value_type("val")));
    EXPECT_THAT(stm.valid(), Eq(true));
  }

  TEST(Test_Statement_Insert, producesInsertSqlCodeTableParams)
  {
    TestTable row;
    row.Key = "key";
    row.Value = "val";

    Insert<TestTable> stm;
    stm.values() | row;
    EXPECT_THAT(stm.string(), StrEq("INSERT INTO TestTable(Key,Value) VALUES(?,?);"));
    EXPECT_THAT(stm.params(), ElementsAre(Params::value_type("key"), Params::value_type("val")));
    EXPECT_THAT(stm.valid(), Eq(true));
  }
} // namespace C8::Db::Statement
