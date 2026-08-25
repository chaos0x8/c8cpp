#include "c8cpp-cipher/asymmetric_key.hpp"
#include "c8cpp-cipher/utility.hpp"

#include <gmock/gmock.h>

namespace C8::Cipher {
  using namespace testing;
  using Utility::toString;

  class TestAssymetricKey : public Test {
  public:
    std::string exampleData = "text";
    AsymmetricKey cut{KeySize::k2};
  };

  TEST_F(TestAssymetricKey, encryptReturnsDifferentBytesEachTime) {
    const auto enc1 = cut.encrypt(exampleData);
    const auto enc2 = cut.encrypt(exampleData);

    EXPECT_THAT(enc1.cview(), Not(Eq(enc2.cview())));
  }

  TEST_F(TestAssymetricKey, decryptEncryptedData) {
    const auto enc1 = cut.encrypt(exampleData);
    const auto dec1 = cut.decrypt(enc1);

    EXPECT_THAT(dec1.cview().string_view(), Eq(exampleData));
  }

  TEST_F(TestAssymetricKey, decryptsWhenCreatedFromPublicKey) {
    AsymmetricKey cut2(fromPublicKey, cut.publicKey());

    const auto enc1 = cut2.encrypt(exampleData);
    const auto dec1 = cut.decrypt(enc1);

    EXPECT_THAT(dec1.cview().string_view(), Eq(exampleData));
  }

  TEST_F(TestAssymetricKey, throwsWhenDecodingWithNotMatchingPrivateKey) {
    AsymmetricKey cut2{KeySize::k2};

    const auto enc1 = cut2.encrypt(exampleData);
    EXPECT_THROW(cut.decrypt(enc1), Error::StandardError);
  }

  TEST_F(TestAssymetricKey, decryptsWhenCreatedFromPrivateKey) {
    AsymmetricKey cut2(fromPrivateKey, cut.privateKey());

    const auto enc1 = cut.encrypt(exampleData);
    const auto dec1 = cut2.decrypt(enc1);

    EXPECT_THAT(dec1.cview().string_view(), Eq(exampleData));
  }

  TEST_F(TestAssymetricKey, throwsWhenDecodingWithNotMatchingPrivateKey_1) {
    AsymmetricKey cut2{KeySize::k2};

    const auto enc1 = cut.encrypt(exampleData);
    EXPECT_THROW(cut2.decrypt(enc1), Error::StandardError);
  }
} // namespace C8::Cipher
