#include "c8cpp-cipher/tls_client.hpp"
#include "c8cpp-cipher/error.hpp"
#include "c8cpp-network/error.hpp"

#include <iostream>

namespace C8::Cipher {
  TlsClient::TlsClient(Network::TcpIpClient socket, ICertStorage& certStorage, ServerMethod_t)
    : socket_(std::move(socket)), ctx_(OsslPointer<SSL_CTX>::create(serverMethod)), ssl_(createSsl(certStorage, ctx_))
  {
    Error::checkSuccess(SSL_set_fd(ssl_.raw(), socket_.raw()) == 1, "SSL_set_fd failed.");
  }

  TlsClient::TlsClient(Network::TcpIpClient socket, ICertStorage& certStorage, ClientMethod_t)
    : socket_(std::move(socket)), ctx_(OsslPointer<SSL_CTX>::create(clientMethod)), ssl_(createSsl(certStorage, ctx_))
  {
    Error::checkSuccess(SSL_set_fd(ssl_.raw(), socket_.raw()) == 1, "SSL_set_fd failed.");
  }

  C8::Network::SocketCoro<> TlsClient::accept()
  {
    return ssl_.accept();
  }

  C8::Network::SocketCoro<> TlsClient::connect()
  {
    return ssl_.connect();
  }

  void TlsClient::sendString(std::string_view str)
  {
    ssl_.sendString(str);
  }

  C8::Network::SocketCoro<std::string> TlsClient::recvString()
  {
    return ssl_.recvString();
  }

  void TlsClient::sendBytes(std::span<const std::byte> bytes)
  {
    ssl_.sendBytes(bytes);
  }

  C8::Network::SocketCoro<std::vector<std::byte>> TlsClient::recvBytes()
  {
    return ssl_.recvBytes();
  }

  void TlsClient::send(const void* ptr, size_t size)
  {
    ssl_.send(ptr, size);
  }

  C8::Network::SocketCoro<void> TlsClient::recv(void* ptr, size_t size)
  {
    return ssl_.recv(ptr, size);
  }

  OsslPointer<SSL> TlsClient::createSsl(ICertStorage& certStorage, OsslPointer<SSL_CTX>& ctx)
  {
    certStorage.configure(ctx);
    return OsslPointer<SSL>::create(ctx);
  }
} // namespace C8::Cipher
