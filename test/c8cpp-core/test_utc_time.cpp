#include "c8cpp-core/error.hpp"
#include "c8cpp-core/utc_time.hpp"

#include <fstream>
#include <regex>

#include <gmock/gmock.h>

namespace C8::Core {
  using namespace testing;

  class TestUtcTime : public Test {
  public:
    void TearDown() override {
      if (std::filesystem::exists(randomFile)) {
        std::filesystem::remove(randomFile);
      }
    }

    std::string timestampRegex = "^[0-9]{4}-[0-9]{2}-[0-9]{2} [0-9]{2}:[0-9]{2}:[0-9]{2}\\.[0-9]{9}$";
    std::filesystem::path randomFile = std::filesystem::temp_directory_path() / "random_file.txt";
  };

  TEST_F(TestUtcTime, createsFromCurrentUtcTime) {
    const auto cut0 = UtcTime::now();
    EXPECT_THAT(cut0.string(), MatchesRegex(timestampRegex));

    const auto cut1 = UtcTime(std::chrono::utc_clock::now());
    EXPECT_THAT(cut1.string(), MatchesRegex(timestampRegex));
  }

  TEST_F(TestUtcTime, createsFromCurrentSystemTime) {
    const auto cut = UtcTime(std::chrono::system_clock::now());
    EXPECT_THAT(cut.string(), MatchesRegex(timestampRegex));
  }

  TEST_F(TestUtcTime, createsFromString) {
    const auto cut0 = UtcTime::now();
    const auto cut1 = UtcTime(cut0.string());

    EXPECT_THAT(cut0.string(), Eq(cut1.string()));
  }

  TEST_F(TestUtcTime, createsFromFileModificationTime) {
    std::ofstream file(randomFile);
    file.close();

    const auto cut = UtcTime::mtime(randomFile);

    EXPECT_THAT(cut.string(), MatchesRegex(timestampRegex));
  }

  TEST_F(TestUtcTime, changesDateByAddingSeconds) {
    const auto cut = UtcTime("2025-03-24 07:00:00.000000000") + std::chrono::seconds(90);

    EXPECT_THAT(cut.string(), Eq("2025-03-24 07:01:30.000000000"));
  }

  TEST_F(TestUtcTime, comparesWithOtherUtcTime) {
    const auto cut0 = UtcTime("2025-03-24 07:00:00.000000000");
    const auto cut1 = UtcTime("2025-03-24 07:00:00.000000000");

    EXPECT_THAT(cut0, Eq(cut1));
    EXPECT_THAT(cut0 + std::chrono::seconds(1), Gt(cut1));
    EXPECT_THAT(cut0, Lt(cut1 + std::chrono::seconds(1)));
    EXPECT_THAT(cut0 - std::chrono::seconds(1), Lt(cut1));
    EXPECT_THAT(cut0, Gt(cut1 - std::chrono::seconds(1)));
  }

  TEST_F(TestUtcTime, throwsWhenConvertingFromInvalidString) {
    EXPECT_THROW(UtcTime("invalid"), Error::ArgumentError);
  }
} // namespace C8::Core
