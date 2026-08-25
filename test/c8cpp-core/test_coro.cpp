#include "c8cpp-core/coro.hpp"

#include <generator>

#include <gmock/gmock.h>

namespace C8::Core {
  using namespace testing;

  struct TestCoro : public Test {};

  TEST_F(TestCoro, coroGenerator)
  {
    auto cut = []() -> Generator<int>
    {
      co_yield 10;
      co_yield 20;
    }();

    std::vector<int> vec;
    while (cut.next()) {
      vec.emplace_back(cut.value());
    }

    EXPECT_THAT(vec, ElementsAre(10, 20));
  }

  TEST_F(TestCoro, coroValueTask)
  {
    auto cut = []() -> Task<int>
    {
      co_await std::suspend_always();

      co_return 32;
    }();

    EXPECT_THAT(cut.done(), Eq(false));
    cut.resume();
    EXPECT_THAT(cut.done(), Eq(false));
    cut.resume();
    EXPECT_THAT(cut.done(), Eq(true));
    EXPECT_THAT(cut.value(), Eq(32));
  }

  TEST_F(TestCoro, coroVoidTask)
  {
    auto cut = []() -> Task<void>
    {
      co_await std::suspend_always();

      co_return;
    }();

    EXPECT_THAT(cut.done(), Eq(false));
    cut.resume();
    EXPECT_THAT(cut.done(), Eq(false));
    cut.resume();
    EXPECT_THAT(cut.done(), Eq(true));
    cut.value();
  }

  TEST_F(TestCoro, coroValueTaskThrowsWhenCheckingValue)
  {
    auto cut = []() -> Task<int>
    {
      throw std::runtime_error("error");

      co_return 42;
    }();

    EXPECT_THAT(cut.done(), Eq(false));
    cut.resume();
    EXPECT_THAT(cut.done(), Eq(true));
    EXPECT_THROW(cut.value(), std::runtime_error);
  }

  TEST_F(TestCoro, coroVoidTaskThrowsWhenCheckingValue)
  {
    auto cut = []() -> Task<void>
    {
      throw std::runtime_error("error");

      co_return;
    }();

    EXPECT_THAT(cut.done(), Eq(false));
    cut.resume();
    EXPECT_THAT(cut.done(), Eq(true));
    EXPECT_THROW(cut.value(), std::runtime_error);
  }
} // namespace C8::Core
