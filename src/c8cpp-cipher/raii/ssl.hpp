#pragma once

#include "c8cpp-cipher/raii/raii.hpp"

namespace C8::Cipher {
  template <>
  struct OsslPointerTraits<SSL> {
    static SSL* create(OsslPointer<SSL_CTX>& sslCtx);
    static void destroy(SSL* ptr);

    virtual ~OsslPointerTraits() = default;

    void set_fd(int fd);
    OsslPointer<X509> get_peer_certificate();
    C8::Network::SocketCoro<> accept();
    C8::Network::SocketCoro<> connect();
    void send(const void* ptr, size_t size);
    C8::Network::SocketCoro<> recv(void* ptr, size_t size);
    void sendString(std::string_view str);
    C8::Network::SocketCoro<std::string> recvString();
    void sendBytes(std::span<const std::byte> bytes);
    C8::Network::SocketCoro<std::vector<std::byte>> recvBytes();
  };
} // namespace C8::Cipher
