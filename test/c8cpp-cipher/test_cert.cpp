#include "c8cpp-cipher/cert.hpp"
#include "c8cpp-cipher/utility.hpp"
#include "c8cpp-core/utc_time.hpp"

#include <gmock/gmock.h>

#include <chrono>
#include <fstream>

namespace C8::Cipher {
  using namespace testing;

  class TestCert : public Test {
  public:
    void TearDown() override
    {
      rm(privateKeyPath);
      rm(publicKeyPath);
    }

    void rm(std::filesystem::path path)
    {
      if (std::filesystem::exists(path)) {
        std::filesystem::remove(path);
      }
    }

    IssuerInfo issuer{.country = "PL", .organization = "SelfSigned", .commonName = "SelfSigned.com"};
    Cert cut{issuer, KeySize::k2, std::chrono::years(1)};
    std::filesystem::path privateKeyPath = std::filesystem::temp_directory_path() / "private";
    std::filesystem::path publicKeyPath = std::filesystem::temp_directory_path() / "public";
  };

  TEST_F(TestCert, publicKeyIsSameAsInFile)
  {
    cut.writePublicKeyFile(publicKeyPath);

    EXPECT_THAT(cut.publicKey().cview().span(), ElementsAreArray(Utility::readFile(publicKeyPath)));
  }

  TEST_F(TestCert, privateKeyIsSameAsInFile)
  {
    cut.writePrivateKeyFile(privateKeyPath);

    EXPECT_THAT(cut.privateKey().cview().span(), ElementsAreArray(Utility::readFile(privateKeyPath)));
  }

  TEST_F(TestCert, createsCertFromFiles)
  {
    cut.writePublicKeyFile(publicKeyPath);
    cut.writePrivateKeyFile(privateKeyPath);

    Cert copy = Cert::fromFile(privateKeyPath, publicKeyPath);

    EXPECT_THAT(copy.privateKey().cview(), cut.privateKey().cview());
    EXPECT_THAT(copy.publicKey().cview(), cut.publicKey().cview());
  }

  TEST_F(TestCert, createsFromMemory)
  {
    Cert copy(cut.privateKey(), cut.publicKey());

    EXPECT_THAT(copy.privateKey().cview(), cut.privateKey().cview());
    EXPECT_THAT(copy.publicKey().cview(), cut.publicKey().cview());
  }

  TEST_F(TestCert, clone_clonesCert)
  {
    const auto copy = cut.clone();
    EXPECT_THAT(copy.privateKey().cview(), cut.privateKey().cview());
    EXPECT_THAT(copy.publicKey().cview(), cut.publicKey().cview());
  }

  TEST_F(TestCert, expirationDate_returnsCertificateExpirationDate)
  {
    const auto formatString = "{0:%F} {0:%H}:00:00";
    const auto expectedExpiration = C8::Core::UtcTime::now() + std::chrono::years(1);
    EXPECT_THAT(cut.expirationDate().format(formatString), StrEq(expectedExpiration.format(formatString)));
    EXPECT_THAT(cut.x509().expirationDate().format(formatString), StrEq(expectedExpiration.format(formatString)));
  }

  TEST_F(TestCert, commonName_returnsCertificateCommonName)
  {
    EXPECT_THAT(cut.commonName(), StrEq(issuer.commonName));
    EXPECT_THAT(cut.x509().commonName(), StrEq(issuer.commonName));
  }
} // namespace C8::Cipher
