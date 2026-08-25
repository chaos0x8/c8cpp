#include "c8cpp-cipher/cert_storage.hpp"
#include "c8cpp-cipher/global_init.hpp"
#include "c8cpp-cipher/tls_client.hpp"
#include "c8cpp-network/socket_selector.hpp"
#include "c8cpp-network/tcp_ip_socket.hpp"

#include <gmock/gmock.h>

namespace C8::Cipher {
  using namespace testing;

  class TestTlsClient : public Test {
  public:
    void setupMtlsEchoServer()
    {
      serverCertStorage = Cipher::createCertStorage(VerifyPeer::yes, Cipher::Cert{serverInfo, Cipher::KeySize::k2});
      client1CertStorage = Cipher::createCertStorage(VerifyPeer::yes, Cipher::Cert{client1Info, KeySize::k2});
      client2CertStorage = Cipher::createCertStorage(VerifyPeer::yes, Cipher::Cert{client2Info, KeySize::k2});

      serverCertStorage->authorize(OsslPointer<X509>::create(*client1CertStorage->publicKey()));
      serverCertStorage->authorize(OsslPointer<X509>::create(*client2CertStorage->publicKey()));
      client1CertStorage->authorize(OsslPointer<X509>::create(*serverCertStorage->publicKey()));
      client2CertStorage->authorize(OsslPointer<X509>::create(*serverCertStorage->publicKey()));

      selector.start();

      selector.insert(
        Network::TcpIpServer{ip, port},
        [](auto& server, auto* selector, auto* serverCertStorage) -> C8::Network::SocketCoro<> {
          while (true) {
            TlsClient client{co_await server.accept(), *serverCertStorage, serverMethod};
            selector->insert(std::move(client), [](auto& client) -> C8::Network::SocketCoro<> {
              co_await client.accept();

              while (true) {
                const std::string txt = co_await client.recvString();
                client.sendString(txt);

                co_await std::suspend_always{};
              }
            });

            co_await std::suspend_always{};
          }
        },
        &selector, serverCertStorage.get());
    }

    void setupTlsEchoServer()
    {
      serverCertStorage = Cipher::createCertStorage(VerifyPeer::no, Cipher::Cert{serverInfo, Cipher::KeySize::k2});
      client1CertStorage = Cipher::createCertStorage(VerifyPeer::yes, std::nullopt);
      client2CertStorage = Cipher::createCertStorage(VerifyPeer::yes, std::nullopt);

      client1CertStorage->authorize(OsslPointer<X509>::create(*serverCertStorage->publicKey()));
      client2CertStorage->authorize(OsslPointer<X509>::create(*serverCertStorage->publicKey()));

      selector.start();

      selector.insert(
        Network::TcpIpServer{ip, port},
        [](auto& server, auto* selector, auto* serverCertStorage) -> C8::Network::SocketCoro<> {
          while (true) {
            TlsClient client{co_await server.accept(), *serverCertStorage, serverMethod};
            selector->insert(std::move(client), [](auto& client) -> C8::Network::SocketCoro<> {
              co_await client.accept();

              while (true) {
                const std::string txt = co_await client.recvString();
                client.sendString(txt);

                co_await std::suspend_always{};
              }
            });

            co_await std::suspend_always{};
          }
        },
        &selector, serverCertStorage.get());
    }

    template <typename T>
    static T resolve(C8::Network::SocketCoro<T> coro)
    {
      while (!coro.done()) {
        coro.resume();
      }

      return coro.value();
    }

    Cipher::IssuerInfo serverInfo{.commonName = "server"};
    Cipher::IssuerInfo client1Info{.commonName = "client1"};
    Cipher::IssuerInfo client2Info{.commonName = "client2"};

    std::string ip = "127.0.0.1";
    std::string port = "4242";
    Cipher::GlobalInit init;
    std::unique_ptr<Cipher::ICertStorage> serverCertStorage;
    std::unique_ptr<Cipher::ICertStorage> client1CertStorage;
    std::unique_ptr<Cipher::ICertStorage> client2CertStorage;
    Network::SocketSelector selector;
  };

  TEST_F(TestTlsClient, sendsAndReceiveMessageInMtlsMode)
  {
    setupMtlsEchoServer();

    TlsClient client1{Network::TcpIpClient{ip, port}, *client1CertStorage, clientMethod};
    TlsClient client2{Network::TcpIpClient{ip, port}, *client2CertStorage, clientMethod};

    resolve(client1.connect());
    resolve(client2.connect());

    client1.sendString("Hello");
    client2.sendString("World");

    EXPECT_THAT(resolve(client1.recvString()), Eq("Hello"));
    EXPECT_THAT(resolve(client2.recvString()), Eq("World"));
  }

  TEST_F(TestTlsClient, refusesConnectionWhenCertNotAuthorizedInMtlsMode)
  {
    setupMtlsEchoServer();

    const Cipher::IssuerInfo client3Info{.commonName = "client3"};
    auto client3CertStorage = Cipher::createCertStorage(VerifyPeer::no, Cipher::Cert{client3Info, KeySize::k2});

    TlsClient client1{Network::TcpIpClient{ip, port}, *client3CertStorage, clientMethod};

    EXPECT_THROW(resolve(client1.connect()), Error::ConnectionRefusedError);
  }

  TEST_F(TestTlsClient, sendsAndReceiveMessageInTlsMode)
  {
    setupTlsEchoServer();

    TlsClient client1{Network::TcpIpClient{ip, port}, *client1CertStorage, clientMethod};
    TlsClient client2{Network::TcpIpClient{ip, port}, *client2CertStorage, clientMethod};

    resolve(client1.connect());
    resolve(client2.connect());

    client1.sendString("Hello");
    client2.sendString("World");

    EXPECT_THAT(resolve(client1.recvString()), Eq("Hello"));
    EXPECT_THAT(resolve(client2.recvString()), Eq("World"));
  }

  TEST_F(TestTlsClient, sendsAndReceiveBytesInTlsMode)
  {
    setupTlsEchoServer();

    TlsClient client1{Network::TcpIpClient{ip, port}, *client1CertStorage, clientMethod};
    TlsClient client2{Network::TcpIpClient{ip, port}, *client2CertStorage, clientMethod};

    resolve(client1.connect());
    resolve(client2.connect());

    client1.sendBytes(std::as_bytes(std::span{"Hello"}));
    client2.sendBytes(std::as_bytes(std::span{"World"}));

    EXPECT_THAT(std::ranges::equal(resolve(client1.recvBytes()), std::as_bytes(std::span{"Hello"})), Eq(true));
    EXPECT_THAT(std::ranges::equal(resolve(client2.recvBytes()), std::as_bytes(std::span{"World"})), Eq(true));
  }
} // namespace C8::Cipher
