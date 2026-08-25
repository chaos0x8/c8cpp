#include "c8cpp-cipher/mols_string.hpp"

#include <gmock/gmock.h>

namespace C8::Cipher {
  using namespace testing;

  class TestMolsString : public Test {
  public:
    std::string secret = "secret";
    std::string secret2 = "secret2";
    std::unique_ptr<MolsString> cut = std::make_unique<MolsString>(secret);
  };

  TEST_F(TestMolsString, valueReturnsOriginalValue) {
    EXPECT_THAT(cut->value().string_view(), Eq(secret));
  }

  TEST_F(TestMolsString, returnNewValueWhenAssigned) {
    cut = std::make_unique<MolsString>(secret2);

    EXPECT_THAT(cut->value().string_view(), Eq(secret2));
  }

  TEST_F(TestMolsString, clonesMolsString) {
    auto other = cut->clone();

    EXPECT_THAT(cut->value(), Eq(other->value()));
  }
} // namespace C8::Cipher
