#include "c8cpp-cipher/raii/ssl.hpp"
#include "c8cpp-cipher/raii/ssl_ctx.hpp"
#include "c8cpp-cipher/raii/x509.hpp"

#include <openssl/err.h>

namespace C8::Cipher {
  SSL* OsslPointerTraits<SSL>::create(OsslPointer<SSL_CTX>& sslCtx)
  {
    return SSL_new(sslCtx.raw());
  }

  void OsslPointerTraits<SSL>::destroy(SSL* ptr)
  {
    SSL_free(ptr);
  }

  void OsslPointerTraits<SSL>::set_fd(int fd)
  {
    C8_ASSERTT(Cipher::Error::AssertError, SSL_set_fd(static_cast<OsslPointer<SSL>&>(*this).raw(), fd) == 1,
      "SSL_set_fd failed");
  }

  OsslPointer<X509> OsslPointerTraits<SSL>::get_peer_certificate()
  {
    return OsslPointer<X509>::create(fromRaw, SSL_get_peer_certificate(static_cast<OsslPointer<SSL>&>(*this).raw()));
  }

  C8::Network::SocketCoro<> OsslPointerTraits<SSL>::accept()
  {
    while (true) {
      const auto ret{SSL_accept(static_cast<OsslPointer<SSL>&>(*this).raw())};
      if (ret == 1) {
        co_return;
      }

      const auto err{SSL_get_error(static_cast<OsslPointer<SSL>&>(*this).raw(), ret)};
      if (err == SSL_ERROR_WANT_READ || err == SSL_ERROR_WANT_WRITE) {
        co_await std::suspend_always{};
        continue;
      }

      break;
    }

    throw Error::ConnectionRefusedError::acceptError();
  }

  C8::Network::SocketCoro<> OsslPointerTraits<SSL>::connect()
  {
    while (true) {
      const auto ret{SSL_connect(static_cast<OsslPointer<SSL>&>(*this).raw())};
      if (ret == 1) {
        co_return;
      }

      const auto err{SSL_get_error(static_cast<OsslPointer<SSL>&>(*this).raw(), ret)};
      if (err == SSL_ERROR_WANT_READ || err == SSL_ERROR_WANT_WRITE) {
        co_await std::suspend_always{};
        continue;
      }

      break;
    }

    throw Error::ConnectionRefusedError::connectError();
  }

  void OsslPointerTraits<SSL>::send(const void* ptr, size_t size)
  {
    while (true) {
      size_t sendSize{};
      const auto ret{SSL_write_ex(static_cast<OsslPointer<SSL>&>(*this).raw(), ptr, size, &sendSize)};

      if (ret == 1) {
        if (sendSize == size) {
          return;
        }

        throw Network::Error::IoError(errno);
      }

      const auto err{SSL_get_error(static_cast<OsslPointer<SSL>&>(*this).raw(), ret)};
      if (err == SSL_ERROR_WANT_READ || err == SSL_ERROR_WANT_WRITE) {
        continue;
      }

      break;
    }

    throw Network::Error::IoError(errno);
  }

  C8::Network::SocketCoro<> OsslPointerTraits<SSL>::recv(void* ptr, size_t size)
  {
    while (true) {
      size_t readSize{};
      const auto ret{SSL_read_ex(static_cast<OsslPointer<SSL>&>(*this).raw(), ptr, size, &readSize)};

      if (ret == 1) {
        if (readSize == size) {
          co_return;
        }

        throw Network::Error::IoError(errno);
      }

      const auto err{SSL_get_error(static_cast<OsslPointer<SSL>&>(*this).raw(), ret)};
      if (err == SSL_ERROR_WANT_READ || err == SSL_ERROR_WANT_WRITE) {
        co_await std::suspend_always{};
        continue;
      }

      break;
    }

    throw Network::Error::IoError(errno);
  }

  void OsslPointerTraits<SSL>::sendString(std::string_view str)
  {
    const uint64_t length = str.size();
    send(&length, sizeof(length));
    send(str.data(), length);
  }

  C8::Network::SocketCoro<std::string> OsslPointerTraits<SSL>::recvString()
  {
    uint64_t length{};
    co_await recv(&length, sizeof(length));

    std::string out(length, '\0');
    co_await recv(out.data(), length);

    co_return out;
  }

  void OsslPointerTraits<SSL>::sendBytes(std::span<const std::byte> bytes)
  {
    const uint64_t length = bytes.size();
    send(&length, sizeof(length));
    send(bytes.data(), length);
  }

  C8::Network::SocketCoro<std::vector<std::byte>> OsslPointerTraits<SSL>::recvBytes()
  {
    uint64_t length{};
    co_await recv(&length, sizeof(length));

    std::vector<std::byte> out(length);
    co_await recv(out.data(), length);

    co_return out;
  }
} // namespace C8::Cipher
