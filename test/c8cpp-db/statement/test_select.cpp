#include "c8cpp-db/statement/select.hpp"
#include "c8cpp-db/test_utility/test_table.hpp"

#include <gmock/gmock.h>

namespace C8::Db::Statement {
  using namespace TestUtility;
  using namespace testing;

  TEST(Test_Statement_Select, producesSelectSqlCodeWithNoParams)
  {
    Select<TestTable> stm;
    EXPECT_THAT(stm.string(), StrEq("SELECT rowid, * FROM TestTable;"));
    EXPECT_THAT(stm.params(), IsEmpty());
    EXPECT_THAT(stm.valid(), Eq(true));
  }

  TEST(Test_Statement_Select, producesSelectSqlCode)
  {
    Select<TestTable> stm;
    stm.where() && (stm.q<^^TestTable::Key>() == "kv");
    EXPECT_THAT(stm.string(), StrEq("SELECT rowid, * FROM TestTable WHERE Key == ?;"));
    EXPECT_THAT(stm.params(), ElementsAre(Params::value_type("kv")));
    EXPECT_THAT(stm.valid(), Eq(true));
  }

  TEST(Test_Statement_Select, producesSelectSqlCodeWithMultiParams)
  {
    Select<TestTable> stm;
    stm.where() && (stm.q<^^TestTable::Key>() == "kv") && (stm.q<^^TestTable::Value>() == "vv");
    EXPECT_THAT(stm.string(), StrEq("SELECT rowid, * FROM TestTable WHERE Key == ? AND Value == ?;"));
    EXPECT_THAT(stm.params(), ElementsAre(Params::value_type("kv"), Params::value_type("vv")));
    EXPECT_THAT(stm.valid(), Eq(true));
  }

  TEST(Test_Statement_Select, recognizesRowidAsAColumn)
  {
    Select<TestTable> stm;
    stm.where() && (stm.q<^^TestTable::rowid>() == 32);
    EXPECT_THAT(stm.string(), StrEq("SELECT rowid, * FROM TestTable WHERE rowid == ?;"));
    EXPECT_THAT(stm.params(), ElementsAre(Params::value_type(32l)));
    EXPECT_THAT(stm.valid(), Eq(true));
  }

} // namespace C8::Db::Statement
