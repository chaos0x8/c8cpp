#include "c8cpp-db/detail/meta.hpp"
#include "c8cpp-db/statement/create_table.hpp"
#include "c8cpp-db/test_utility/test_table.hpp"

#include <gmock/gmock.h>

#include <meta>

namespace C8::Db::Statement {
  using namespace TestUtility;
  using namespace testing;

  TEST(Test_Statement_CreateTable, producesCreateTebleSqlCode)
  {
    CreateTable<TestTable> stm;
    EXPECT_THAT(stm.string(), StrEq("CREATE TABLE TestTable(Key string, Value string);"));
    EXPECT_THAT(stm.params(), IsEmpty());
    EXPECT_THAT(stm.valid(), Eq(true));
  }

  TEST(Test_Statement_CreateTable, producesCreateIfNotExistsTebleSqlCode)
  {
    CreateTable<TestTable> stm;
    stm.ifNotExists();
    EXPECT_THAT(stm.string(), StrEq("CREATE TABLE IF NOT EXISTS TestTable(Key string, Value string);"));
    EXPECT_THAT(stm.params(), IsEmpty());
    EXPECT_THAT(stm.valid(), Eq(true));
  }
} // namespace C8::Db::Statement
