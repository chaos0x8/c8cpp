#include "c8cpp-cipher/cert.hpp"
#include "c8cpp-cipher/cert_storage.hpp"

#include <gmock/gmock.h>

#include <fstream>

namespace C8::Cipher {
  using namespace testing;

  class TestCertStorage : public Test {
  public:
    IssuerInfo issuer{};
    Cert cert{issuer, KeySize::k2};
    std::unique_ptr<ICertStorage> cut = createCertStorage(std::move(cert));
  };

  TEST_F(TestCertStorage, authorizesPublicKey) {
    Cert clientCert{issuer, KeySize::k2};
    cut->authorize(OsslPointer<X509>::create(clientCert.publicKey()));
  }
} // namespace C8::Cipher
