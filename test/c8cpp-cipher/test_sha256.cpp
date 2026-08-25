#include "c8cpp-cipher/sha256.hpp"

#include <gmock/gmock.h>

namespace C8::Cipher {
  using namespace testing;

  class TestSha256 : public Test {
  public:
  };

  TEST_F(TestSha256, hashesString) {
    EXPECT_THAT(Sha256::Hash("hello\n").dump(), Eq("5891b5b522d5df086d0ff0b110fbd9d21bb4fc7163af34d08286a2e846f6be03"));
  }

  TEST_F(TestSha256, twoSameStringsProduceSameHash) {
    const auto hash0 = Sha256::Hash("hello");
    const auto hash1 = Sha256::Hash("hello");
    EXPECT_THAT(hash0, Eq(hash1));
    EXPECT_THAT(hash0.value(), Eq(hash1.value()));
  }
} // namespace C8::Cipher
