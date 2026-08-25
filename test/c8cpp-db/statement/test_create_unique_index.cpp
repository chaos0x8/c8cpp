#include "c8cpp-db/statement/create_unique_index.hpp"
#include "c8cpp-db/test_utility/test_table.hpp"

#include <gmock/gmock.h>

namespace C8::Db::Statement {
  using namespace TestUtility;
  using namespace testing;

  TEST(Test_Statement_CreateUniqueIndex, producesCreateUniqueIndexSqlCode)
  {
    CreateUniqueIndex<TestTable> stm;
    stm.index() | stm.q<^^TestTable::Key>();
    EXPECT_THAT(stm.string(), StrEq("CREATE UNIQUE INDEX TestTable_Index ON TestTable (Key)"));
    EXPECT_THAT(stm.params(), IsEmpty());
    EXPECT_THAT(stm.valid(), Eq(true));
  }

  TEST(Test_Statement_CreateUniqueIndex, producesCreateUniqueIfNotExistsIndexSqlCode)
  {
    CreateUniqueIndex<TestTable> stm;
    stm.index() | stm.q<^^TestTable::Key>();
    stm.ifNotExists();
    EXPECT_THAT(stm.string(), StrEq("CREATE UNIQUE INDEX IF NOT EXISTS TestTable_Index ON TestTable (Key)"));
    EXPECT_THAT(stm.params(), IsEmpty());
    EXPECT_THAT(stm.valid(), Eq(true));
  }

  TEST(Test_Statement_CreateUniqueIndex, producesCreateUniqueIndexSqlCode_forMoreThanOneColumn)
  {
    CreateUniqueIndex<TestTable> stm;
    stm.index() | stm.q<^^TestTable::Key>() | stm.q<^^TestTable::Value>();
    EXPECT_THAT(stm.string(), StrEq("CREATE UNIQUE INDEX TestTable_Index ON TestTable (Key,Value)"));
    EXPECT_THAT(stm.params(), IsEmpty());
    EXPECT_THAT(stm.valid(), Eq(true));
  }

  TEST(Test_Statement_CreateUniqueIndex, producesCreateUniqueIfNotExistsIndexSqlCode_forMoreThanOneColumn)
  {
    CreateUniqueIndex<TestTable> stm;
    stm.index() | stm.q<^^TestTable::Key>() | stm.q<^^TestTable::Value>();
    stm.ifNotExists();
    EXPECT_THAT(stm.string(), StrEq("CREATE UNIQUE INDEX IF NOT EXISTS TestTable_Index ON TestTable (Key,Value)"));
    EXPECT_THAT(stm.params(), IsEmpty());
    EXPECT_THAT(stm.valid(), Eq(true));
  }
} // namespace C8::Db::Statement
