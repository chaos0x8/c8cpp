#pragma once

#include "c8cpp-cipher/cert_storage.hpp"
#include "c8cpp-network/tcp_ip_socket.hpp"

namespace C8::Cipher {
  using AuthorizedKeys = std::vector<OsslPointer<X509>>;

  class TlsClient {
  public:
    TlsClient(Network::TcpIpClient socket, ICertStorage&, ServerMethod_t);
    TlsClient(Network::TcpIpClient socket, ICertStorage&, ClientMethod_t);

    C8::Network::SocketCoro<void> accept();
    C8::Network::SocketCoro<void> connect();

    void sendString(std::string_view);
    C8::Network::SocketCoro<std::string> recvString();

    void sendBytes(std::span<const std::byte>);
    C8::Network::SocketCoro<std::vector<std::byte>> recvBytes();

    void send(const void*, size_t);
    C8::Network::SocketCoro<void> recv(void*, size_t);

    Network::FileDescriptor::value_type raw() const
    {
      return socket_.raw();
    }

  private:
    static OsslPointer<SSL> createSsl(ICertStorage&, OsslPointer<SSL_CTX>&);

    Network::TcpIpClient socket_;
    OsslPointer<SSL_CTX> ctx_;
    OsslPointer<SSL> ssl_;
  };
}; // namespace C8::Cipher
