#include "c8cpp-core/meta.hpp"

#include <gmock/gmock.h>

namespace C8::Core {
  using namespace testing;

  struct TestMeta : public Test {
  };

  enum class Type { A, B };

  TEST_F(TestMeta, test_enumToString)
  {
    EXPECT_THAT(enumToString(Type::A), StrEq("Type::A"));
    EXPECT_THAT(enumToString(Type::B), StrEq("Type::B"));
  }

  TEST_F(TestMeta, test_stringToEnum)
  {
    EXPECT_THAT(stringToEnum<Type>("Type::A"), Eq(Type::A));
    EXPECT_THAT(stringToEnum<Type>("Type::B"), Eq(Type::B));
  }
} // namespace C8::Core
