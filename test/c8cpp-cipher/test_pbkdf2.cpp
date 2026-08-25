#include "c8cpp-cipher/pbkdf2.hpp"

#include <gmock/gmock.h>

namespace C8::Cipher {
  using namespace testing;

  class TestPbkdf2 : public Test {
  public:
    Pbkdf2::Config config{.saltSize = 16, .keySize = 32};
  };

  TEST_F(TestPbkdf2, twoSameStringsProduceDifferentHash) {
    const auto hash0 = Pbkdf2::Hash(config, "hello");
    const auto hash1 = Pbkdf2::Hash(config, "hello");
    EXPECT_THAT(hash0.dump(), Not(Eq(hash1.dump())));
    EXPECT_THAT(hash0.salt(), Not(Eq(hash1.salt())));
    EXPECT_THAT(hash0.key(), Not(Eq(hash1.key())));
    EXPECT_THAT(hash0, Not(Eq(hash1)));
  }

  TEST_F(TestPbkdf2, returnsAssociatedConfig) {
    const auto hash = Pbkdf2::Hash(config, "hello");
    EXPECT_THAT(hash.config(), Eq(config));
  }

  TEST_F(TestPbkdf2, dumpsHashToHexString) {
    const auto hash = Pbkdf2::Hash(config, "hello");
    EXPECT_THAT(hash.dump(), MatchesRegex("[a-f0-9]+"));
  }

  TEST_F(TestPbkdf2, createsHashFromDumpedHexString) {
    const auto hash0 = Pbkdf2::Hash(config, "hello");
    const auto hash1 = Pbkdf2::Hash(fromDump, hash0.dump());
    EXPECT_THAT(hash0.dump(), Eq(hash1.dump()));
    EXPECT_THAT(hash0.salt(), Eq(hash1.salt()));
    EXPECT_THAT(hash0.key(), Eq(hash1.key()));
    EXPECT_THAT(hash0, Eq(hash1));
  }

  TEST_F(TestPbkdf2, compare_string_equality_equalToSameString) {
    const auto hash = Pbkdf2::Hash(config, "hello");

    EXPECT_THAT(hash == "hello", Eq(true));
    EXPECT_THAT(hash != "hello", Eq(false));
    EXPECT_THAT(hash == "hi", Eq(false));
    EXPECT_THAT(hash != "hi", Eq(true));
  }

  TEST_F(TestPbkdf2, compare_dump_equality_equalToOriginalHashDump) {
    const auto hash0 = Pbkdf2::Hash(config, "hello");
    const auto hash1 = Pbkdf2::Hash(config, "hi");
    const auto hash2 = Pbkdf2::Hash(config, "hello");

    EXPECT_THAT(hash0 == Pbkdf2::Hash(fromDump, hash0.dump()), Eq(true));
    EXPECT_THAT(hash0 != Pbkdf2::Hash(fromDump, hash0.dump()), Eq(false));

    EXPECT_THAT(hash1 == Pbkdf2::Hash(fromDump, hash0.dump()), Eq(false));
    EXPECT_THAT(hash1 != Pbkdf2::Hash(fromDump, hash0.dump()), Eq(true));

    EXPECT_THAT(hash2 == Pbkdf2::Hash(fromDump, hash0.dump()), Eq(false));
    EXPECT_THAT(hash2 != Pbkdf2::Hash(fromDump, hash0.dump()), Eq(true));
  }

  TEST_F(TestPbkdf2, compare_hash_equality_alwaysDiffersFromOtherHashes) {
    const auto hash = Pbkdf2::Hash(config, "hello");

    EXPECT_THAT(hash == Pbkdf2::Hash(config, "hello"), Eq(false));
    EXPECT_THAT(hash != Pbkdf2::Hash(config, "hello"), Eq(true));
  }
} // namespace C8::Cipher
