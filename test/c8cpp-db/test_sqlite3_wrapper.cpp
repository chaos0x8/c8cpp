#include "c8cpp-db/sqlite3_wrapper.hpp"

#include "c8cpp-db/statement.hpp"
#include "c8cpp-db/test_utility/test_table.hpp"

#include <gmock/gmock.h>

namespace C8::Db {
  using namespace TestUtility;
  using namespace testing;

  namespace {
    template <typename T>
    std::vector<int64_t> collectRowIds(const std::vector<T>& collection)
    {
      std::vector<int64_t> result;
      for (const auto& o : collection) {
        result.emplace_back(o.rowid);
      }
      return result;
    }
  } // namespace

  class TestSqLite3Wrapper : public Test {
  public:
    void SetUp() override
    {
      cut->setInMomeryJournal();

      Statement::CreateTable<TestTable> stm1;
      cut->exec(stm1);

      Statement::CreateTable<TestTableEx> stm2;
      cut->exec(stm2);
    }

    void TearDown() override
    {
      cut = nullptr;

      if (std::filesystem::exists(dbPath)) {
        std::filesystem::remove(dbPath);
      }
    }

    std::vector<TestTable> addTreeRows()
    {
      std::vector<TestTable> rows = {{TestTable("K1", "Value1"), TestTable("K2", "Value2"), TestTable("K3", "Value3")}};
      for (auto& row : rows) {
        row.rowid = cut->insert(row);
      }
      return rows;
    }

    std::filesystem::path dbPath = "tygrys.db";
    std::unique_ptr<SqLite3Wrapper> cut = std::make_unique<SqLite3Wrapper>(dbPath);
  };

  TEST_F(TestSqLite3Wrapper, insertsRow)
  {
    auto data = TestTableEx("Version", std::numeric_limits<int32_t>::min(), std::numeric_limits<int64_t>::max(), 4.42);
    data.rowid = cut->insert(data);

    std::vector<TestTableEx> records;
    Statement::Select<TestTableEx> stm;
    for (auto row : cut->exec(stm)) {
      records.emplace_back(std::move(row));
    }

    EXPECT_THAT(records, ElementsAre(data));
    EXPECT_THAT(collectRowIds(records), ElementsAre(data.rowid));
  }

  TEST_F(TestSqLite3Wrapper, failsToInsertDuplicateWhenUniqueIndexIsCreated)
  {
    Statement::CreateUniqueIndex<TestTableEx> stm;
    stm.index() | stm.q<^^TestTableEx::Text>();
    cut->exec(stm);

    auto data = TestTableEx("Version", std::numeric_limits<int32_t>::min(), std::numeric_limits<int64_t>::max(), 4.42);
    data.rowid = cut->insert(data);
    EXPECT_THROW(cut->insert(data), Error::SqLite3Error);

    const auto records = cut->collect(Statement::Select<TestTableEx>{});

    EXPECT_THAT(records, ElementsAre(data));
    EXPECT_THAT(collectRowIds(records), ElementsAre(data.rowid));
  }

  TEST_F(TestSqLite3Wrapper, selectAllRows)
  {
    const auto rows = addTreeRows();

    std::vector<TestTable> records;

    for (auto row : cut->exec(Statement::Select<TestTable>{})) {
      records.emplace_back(std::move(row));
    }

    EXPECT_THAT(records, ElementsAreArray(rows));
    EXPECT_THAT(collectRowIds(records), ElementsAre(1, 2, 3));
  }

  TEST_F(TestSqLite3Wrapper, selectSpecifiedRows)
  {
    const auto rows = addTreeRows();

    std::vector<TestTable> records;

    Statement::Select<TestTable> stm;
    stm.where() && (stm.q<^^TestTable::Key>() == "K2");
    for (auto row : cut->exec(stm)) {
      records.emplace_back(std::move(row));
    }

    EXPECT_THAT(records, ElementsAre(rows[1]));
    EXPECT_THAT(collectRowIds(records), ElementsAre(rows[1].rowid));
  }

  TEST_F(TestSqLite3Wrapper, updateSpecifiedRows)
  {
    auto rows = addTreeRows();

    Statement::Update<TestTable> stm;
    stm.set() | (stm.q<^^TestTable::Value>() = "XXX");
    stm.where() && (stm.q<^^TestTable::Key>() == "K3");
    cut->exec(stm);

    rows[2].Value = "XXX";

    EXPECT_THAT(cut->collect(Statement::Select<TestTable>()), ElementsAreArray(rows));
  }

  TEST_F(TestSqLite3Wrapper, findsSpecifiedRow)
  {
    const auto rows = addTreeRows();

    Statement::Select<TestTable> stm1;
    stm1.where() && (stm1.q<^^TestTable::rowid>() > 1);
    EXPECT_THAT(cut->find(stm1), Eq(rows[1]));

    Statement::Select<TestTable> stm2;
    stm2.where() && (stm2.q<^^TestTable::Key>() == "K3");
    EXPECT_THAT(cut->find(stm2), Eq(rows[2]));

    Statement::Select<TestTable> stm3;
    stm3.where() && (stm3.q<^^TestTable::Key>() == "K1");
    EXPECT_THAT(cut->find(stm3), Eq(rows[0]));
  }

  TEST_F(TestSqLite3Wrapper, deletesSpecifiedRow)
  {
    const auto rows = addTreeRows();

    Statement::Delete<TestTable> stm;
    stm.where() && (stm.q<^^TestTable::rowid>() == rows[1].rowid);
    cut->exec(stm);

    const std::vector<TestTable> records = cut->collect(Statement::Select<TestTable>());
    EXPECT_THAT(collectRowIds(records), ElementsAre(rows[0].rowid, rows[2].rowid));
  }

  TEST_F(TestSqLite3Wrapper, throwsWhenFindCannotFindRow)
  {
    const auto rows = addTreeRows();

    Statement::Select<TestTable> stm;
    stm.where() && (stm.q<^^TestTable::rowid>() > 42);
    EXPECT_THROW(cut->find(stm), Error::RowNotFoundError);
  }

  TEST_F(TestSqLite3Wrapper, collectSpecifiedRows)
  {
    const auto rows = addTreeRows();

    Statement::Select<TestTable> stm;
    stm.where() && (stm.q<^^TestTable::Key>() == "K2");
    const std::vector<TestTable> records = cut->collect(stm);

    EXPECT_THAT(records, ElementsAre(rows[1]));
    EXPECT_THAT(collectRowIds(records), ElementsAre(rows[1].rowid));
  }

  TEST_F(TestSqLite3Wrapper, countsRows)
  {
    const auto rows = addTreeRows();

    Statement::Select<TestTable> stm1;
    EXPECT_THAT(cut->count(stm1), Eq(3));

    Statement::Select<TestTable> stm2;
    stm2.where() && (stm2.q<^^TestTable::Key>() == "K2");
    EXPECT_THAT(cut->count(stm2), Eq(1));
  }

  TEST_F(TestSqLite3Wrapper, throwsWhenCreatingAlreadyExistingTable)
  {
    Statement::CreateTable<TestTable> stm;
    EXPECT_THROW(cut->exec(stm), Error::SqLite3Error);
  }

  TEST_F(TestSqLite3Wrapper, throwsWhenSelectingFromNonExistingTable)
  {
    EXPECT_THROW(cut->collect(Statement::Select<TestTable2>()), Error::SqLite3Error);
  }

  TEST_F(TestSqLite3Wrapper, doesNothingWhenTableIsAlreadyCreated)
  {
    Statement::CreateTable<TestTable> stm;
    stm.ifNotExists();
    cut->exec(stm);
  }

  TEST_F(TestSqLite3Wrapper, createsTable)
  {
    Statement::CreateTable<TestTable2> stm;
    cut->exec(stm);

    EXPECT_THAT(cut->collect(Statement::Select<TestTable2>()), ElementsAre());
  }

  TEST_F(TestSqLite3Wrapper, insertStatementWorksWithCustomType)
  {
    Statement::CreateTable<TestTable3> stm1;
    cut->exec(stm1);

    Statement::Insert<TestTable3> stm2;
    stm2.values() | (stm2.q<^^TestTable3::Value>() = EnumString::Value1);

    TestTable3 row;
    row.Value = EnumString::Value1;
    row.rowid = cut->exec(stm2);

    Statement::Select<TestTable3> stm3;
    stm3.where() && (stm3.q<^^TestTable::rowid>() == row.rowid);
    EXPECT_THAT(cut->find(stm3), Eq(row));
  }

  TEST_F(TestSqLite3Wrapper, insertWorksWithCustomType)
  {
    Statement::CreateTable<TestTable3> stm1;
    cut->exec(stm1);

    TestTable3 row;
    row.Value = EnumString::Value1;
    row.rowid = cut->insert(row);

    Statement::Select<TestTable3> stm2;
    stm2.where() && (stm2.q<^^TestTable3::rowid>() == row.rowid);
    EXPECT_THAT(cut->find(stm2), Eq(row));
  }

  TEST_F(TestSqLite3Wrapper, queryWorksWithCustomType)
  {
    Statement::CreateTable<TestTable3> stm1;
    cut->exec(stm1);

    TestTable3 row;
    row.Value = EnumString::Value1;
    row.rowid = cut->insert(row);

    Statement::Select<TestTable3> stm2;
    stm2.where() && (stm2.q<^^TestTable3::Value>() = EnumString::Value1);
    EXPECT_THAT(cut->find(stm2), Eq(row));
  }

  TEST_F(TestSqLite3Wrapper, insertWorksWithVectors)
  {
    Statement::Insert<TestTableEx> stm1;
    stm1.values() | (stm1.q<^^TestTableEx::Numbers>() = std::vector<int64_t>{{1, 4, 9}});
    stm1.values() |
      (stm1.q<^^TestTableEx::Enums>() = std::vector<EnumString>{{EnumString::Value2, EnumString::Value1}});
    const auto rowid = cut->exec(stm1);

    Statement::Select<TestTableEx> stm2;
    stm2.where() && (stm2.q<^^TestTableEx::rowid>() == rowid);
    const auto row = cut->find(stm2);

    EXPECT_THAT(row.Numbers, ElementsAre(1, 4, 9));
    EXPECT_THAT(row.Enums, ElementsAre(EnumString::Value2, EnumString::Value1));
  }
} // namespace C8::Db
