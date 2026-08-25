#include "c8cpp-core/logger.hpp"

#include <sstream>

#include <gmock/gmock.h>

namespace C8::Core {
  using namespace testing;

  struct TestLogger : public Test {
    std::stringstream ss;
    Logger cut{ss};
  };

  TEST_F(TestLogger, logsMessageWithSourceLocation)
  {
    cut.log(LogLevel::Info, std::format("test {} {}", "tygrys", 42));

    EXPECT_THAT(ss.str(), StrEq("[LogLevel::Info] <c8cpp-core/test_logger.cpp:17> test tygrys 42\n"));
  }
} // namespace C8::Core
