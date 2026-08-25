#include "c8cpp-cipher/aes_cipher.hpp"
#include "c8cpp-cipher/error.hpp"

#include <gmock/gmock.h>

namespace C8::Cipher {
  using namespace testing;

  class TestAesCipher : public Test {
  public:
    std::string password = "password";
    std::string exampleData = "text";
    AesCipher cut;
  };

  TEST_F(TestAesCipher, encryptReturnsDifferentBytesEachTime) {
    const auto enc1 = cut.encrypt(exampleData, password);
    const auto enc2 = cut.encrypt(exampleData, password);

    EXPECT_THAT(enc1.cview(), Not(Eq(enc2.cview())));
  }

  TEST_F(TestAesCipher, decryptEncryptedData) {
    const auto enc1 = cut.encrypt(exampleData, password);
    const auto dec1 = cut.decrypt(enc1, password);

    EXPECT_THAT(dec1.view().string_view(), Eq(exampleData));
  }

  TEST_F(TestAesCipher, decryptFailsWhenWrongPasswordIsProvided) {
    const auto enc1 = cut.encrypt(exampleData, password);

    EXPECT_THROW(cut.decrypt(enc1, "wrong password"), Error::StandardError);
  }
} // namespace C8::Cipher
