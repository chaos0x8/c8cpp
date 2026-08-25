#include "c8cpp-db/statement/update.hpp"
#include "c8cpp-db/test_utility/test_table.hpp"

#include <gmock/gmock.h>

namespace C8::Db::Statement {
  using namespace TestUtility;
  using namespace testing;

  TEST(Test_Statement_Update, producesUpdateSqlCodeWithNoWhereParams)
  {
    Update<TestTable> stm;
    stm.set() | (stm.q<^^TestTable::Value>() = "42");
    EXPECT_THAT(stm.string(), StrEq("UPDATE TestTable SET Value = ?;"));
    EXPECT_THAT(stm.params(), ElementsAre(Params::value_type("42")));
    EXPECT_THAT(stm.valid(), Eq(true));
  }

  TEST(Test_Statement_Update, producesUpdateSqlCode)
  {
    Update<TestTable> stm;
    stm.set() | (stm.q<^^TestTable::Value>() = "42");
    stm.where() && (stm.q<^^TestTable::Key>() == "k");
    EXPECT_THAT(stm.string(), StrEq("UPDATE TestTable SET Value = ? WHERE Key == ?;"));
    EXPECT_THAT(stm.params(), ElementsAre(Params::value_type("42"), Params::value_type("k")));
    EXPECT_THAT(stm.valid(), Eq(true));
  }

  TEST(Test_Statement_Update, producesUpdateSqlCodeWithNoWhereParamsMultiParams)
  {
    Update<TestTable> stm;
    stm.set() | (stm.q<^^TestTable::Value>() = "42") | (stm.q<^^TestTable::Key>() = "7");
    EXPECT_THAT(stm.string(), StrEq("UPDATE TestTable SET Value = ?,Key = ?;"));
    EXPECT_THAT(stm.params(), ElementsAre(Params::value_type("42"), Params::value_type("7")));
    EXPECT_THAT(stm.valid(), Eq(true));
  }

  TEST(Test_Statement_Update, producesUpdateSqlCodeMultiParams)
  {
    Update<TestTable> stm;
    stm.set() | (stm.q<^^TestTable::Value>() = "42") | (stm.q<^^TestTable::Key>() = "7");
    stm.where() && (stm.q<^^TestTable::Key>() == "k") && (stm.q<^^TestTable::Value>() == "2");
    EXPECT_THAT(stm.string(), StrEq("UPDATE TestTable SET Value = ?,Key = ? WHERE Key == ? AND Value == ?;"));
    EXPECT_THAT(stm.params(),
      ElementsAre(Params::value_type("42"), Params::value_type("7"), Params::value_type("k"), Params::value_type("2")));
    EXPECT_THAT(stm.valid(), Eq(true));
  }

  TEST(Test_Statement_Update, producesUpdateSqlCodeTableParams)
  {
    TestTable row;
    row.Key = "42";
    row.Value = "7";

    Update<TestTable> stm;
    stm.set() | row;
    stm.where() && (stm.q<^^TestTable::Key>() == "k") && (stm.q<^^TestTable::Value>() == "2");
    EXPECT_THAT(stm.string(), StrEq("UPDATE TestTable SET Key = ?,Value = ? WHERE Key == ? AND Value == ?;"));
    EXPECT_THAT(stm.params(),
      ElementsAre(Params::value_type("42"), Params::value_type("7"), Params::value_type("k"), Params::value_type("2")));
    EXPECT_THAT(stm.valid(), Eq(true));
  }

  TEST(Test_Statement_Update, recognizesRowidAsAColumn)
  {
    Update<TestTable> stm;
    stm.set() && (stm.q<^^TestTable::Key>() = "x");
    stm.where() && (stm.q<^^TestTable::rowid>() == 32);
    EXPECT_THAT(stm.string(), StrEq("UPDATE TestTable SET Key = ? WHERE rowid == ?;"));
    EXPECT_THAT(stm.params(), ElementsAre(Params::value_type("x"), Params::value_type(32l)));
    EXPECT_THAT(stm.valid(), Eq(true));
  }
} // namespace C8::Db::Statement
