#include "c8cpp-core/algorithm.hpp"

#include <generator>

#include <gmock/gmock.h>

namespace C8::Core {
  using namespace testing;
  using namespace std::string_view_literals;

  struct TestAlgorithm : public Test {};

  TEST_F(TestAlgorithm, join_returnsJoinedString) {
    EXPECT_THAT(join({{"one"sv, "two"sv, "three"sv}}, " "sv), Eq("one two three"));
    EXPECT_THAT(join({{"a"sv, "b"sv, "c"sv}}, " + "sv), Eq("a + b + c"));
  }

  TEST_F(TestAlgorithm, transform_returnsTransformedCollectionAsVector) {
    auto numbers = []() -> std::generator<int> {
      co_yield 3;
      co_yield 42;
      co_yield 15;
    };

    EXPECT_THAT(Ranges::transform(numbers(), [](auto x) { return std::to_string(x); }), ElementsAre("3", "42", "15"));
  }
} // namespace C8::Core
