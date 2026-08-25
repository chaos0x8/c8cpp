#include "c8cpp-core/assert.hpp"
#include "c8cpp-core/error.hpp"

#include <gmock/gmock.h>

namespace C8::Core {
  TEST(TestAssert, doesNothingWhenConditionIsTrue)
  {
    C8_ASSERT(true, "Message");
    C8_ASSERT(true, "Message {}", 42);
  }

  TEST(TestAssert, throwsWhenConditionIsFalse)
  {
    EXPECT_THROW(C8_ASSERT(false, "Message"), Error::AssertError);
    EXPECT_THROW(C8_ASSERT(false, "Message {}", 42), Error::AssertError);
  }

  TEST(TestAssert, alwaysThrows)
  {
    EXPECT_THROW(C8_ABORT("Message"), Error::AssertError);
    EXPECT_THROW(C8_ABORT("Message {}", 42), Error::AssertError);
  }
} // namespace C8::Core
