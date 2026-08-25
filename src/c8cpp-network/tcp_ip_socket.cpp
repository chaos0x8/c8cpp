#include "c8cpp-network/tcp_ip_socket.hpp"
#include "c8cpp-network/error.hpp"

#include <fcntl.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <memory>
#include <thread>

namespace C8::Network {
  namespace {
    struct AddrInfoDeleter {
      void operator()(addrinfo* p)
      {
        freeaddrinfo(p);
      }
    };

    using AddrInfoPtr = std::unique_ptr<addrinfo, AddrInfoDeleter>;

    struct IncompleteSocket {
      FileDescriptor fd;
      AddrInfoPtr addrInfo;
    };

    IncompleteSocket createSocket(const std::string& ip, const std::string& port, __socket_type socketType)
    {
      addrinfo hints{};
      hints.ai_family = AF_UNSPEC;    //! Allows IPv4 or IPv6
      hints.ai_socktype = socketType; //! Basicly TcpIp or Udp
      hints.ai_flags = AI_PASSIVE;    //! Wildcard IP address
      hints.ai_protocol = 0;          //! Any

      addrinfo* address{nullptr};
      if (::getaddrinfo(ip.c_str(), port.c_str(), &hints, &address) != 0) {
        throw Error::SystemError(errno);
      }

      IncompleteSocket result;
      result.addrInfo.reset(address);
      result.fd = FileDescriptor{::socket(address->ai_family, address->ai_socktype, address->ai_protocol)};
      if (!result.fd) {
        throw Error::SystemError(errno);
      }

      return result;
    }

    FileDescriptor bindSocket(const std::string& ip, const std::string port, __socket_type socketType)
    {
      auto r = createSocket(ip, port, socketType);

      int yes = 1;
      if (::setsockopt(r.fd.raw(), SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
        throw Error::SystemError(errno);
      }

      if (::bind(r.fd.raw(), r.addrInfo->ai_addr, r.addrInfo->ai_addrlen) == -1) {
        throw Error::SystemError(errno);
      }

      return std::move(r.fd);
    }

    SocketCoro<FileDescriptor> sys_accept(TcpIpSocket& sock)
    {
      bool loop = true;

      while (loop) {
        loop = false;

        if (auto ret{::accept(sock.raw(), nullptr, nullptr)}; ret != -1) {
          co_return FileDescriptor{ret};
        }

        if (errno == EAGAIN) {
          loop = true;
          co_await std::suspend_always{};
        }
      }

      throw Error::SystemError(errno);
    }

    void sys_listen(FileDescriptor& fd)
    {
      if (::listen(fd.raw(), SOMAXCONN) == -1) {
        throw Error::SystemError(errno);
      }
    }

    void sys_connect(FileDescriptor& fd, const auto& ai_addr, const auto& ai_addrlen)
    {
      if (::connect(fd.raw(), ai_addr, ai_addrlen) == -1) {
        throw Error::SystemError(errno);
      }
    }

    auto sys_fcntl(FileDescriptor& fd, auto opt, auto arg)
    {
      if (auto ret{::fcntl(fd.raw(), opt, arg)}; ret != -1) {
        return ret;
      }

      throw Error::SystemError(errno);
    }

    void setNonBlockingMode(FileDescriptor& fd)
    {
      sys_fcntl(fd, F_SETFL, sys_fcntl(fd, F_GETFL, 0) | O_NONBLOCK);
    }
  } // namespace

  TcpIpSocket TcpIpSocket::server(const std::string& ip, const std::string& port)
  {
    auto fd = bindSocket(ip, port, SOCK_STREAM);

    sys_listen(fd);
    setNonBlockingMode(fd);

    return TcpIpSocket(std::move(fd));
  }

  TcpIpSocket TcpIpSocket::connect(const std::string& ip, const std::string& port)
  {
    auto r = createSocket(ip, port, SOCK_STREAM);

    sys_connect(r.fd, r.addrInfo->ai_addr, r.addrInfo->ai_addrlen);
    setNonBlockingMode(r.fd);

    return TcpIpSocket(std::move(r.fd));
  }

  TcpIpSocket::TcpIpSocket(TcpIpSocket&& other) : fd_(std::move(other.fd_)) {}

  TcpIpSocket& TcpIpSocket::operator=(TcpIpSocket&& other)
  {
    reset();
    std::swap(fd_, other.fd_);
    return *this;
  }

  void TcpIpSocket::reset()
  {
    fd_.reset();
  }

  TcpIpSocket::TcpIpSocket(FileDescriptor fd) : fd_(std::move(fd)) {}

  FileDescriptor::value_type TcpIpSocket::raw() const
  {
    return fd_.raw();
  }

  TcpIpClient::TcpIpClient(TcpIpSocket socket) : socket_(std::move(socket)) {}

  TcpIpClient::TcpIpClient(const std::string& ip, const std::string& port) : socket_(TcpIpSocket::connect(ip, port)) {}

  void TcpIpClient::sendString(std::string_view str)
  {
    const uint64_t length = str.size();
    send(&length, sizeof(length));
    send(str.data(), length);
  }

  SocketCoro<std::string> TcpIpClient::recvString()
  {
    uint64_t length{};
    co_await recv(&length, sizeof(length));

    std::string out(length, '\0');
    co_await recv(out.data(), length);

    co_return out;
  }

  void TcpIpClient::sendBytes(std::span<const std::byte> buffer)
  {
    const uint64_t length = buffer.size();
    send(&length, sizeof(length));
    send(buffer.data(), length);
  }

  SocketCoro<std::vector<std::byte>> TcpIpClient::recvBytes()
  {
    uint64_t length{};
    co_await recv(&length, sizeof(length));

    std::vector<std::byte> out(length);
    co_await recv(out.data(), length);

    co_return out;
  }

  void TcpIpClient::send(std::span<const std::byte> buffer)
  {
    send(reinterpret_cast<const void*>(buffer.data()), buffer.size());
  }

  SocketCoro<void> TcpIpClient::recv(std::span<std::byte> buffer)
  {
    co_await recv(reinterpret_cast<void*>(buffer.data()), buffer.size());
  }

  void TcpIpClient::send(const void* ptr, size_t size)
  {
    while (true) {
      const auto ret{::send(socket_.raw(), ptr, size, 0)};

      if (ret < 0 && errno == EAGAIN) {
        std::this_thread::yield();
      } else if (static_cast<size_t>(ret) == size) {
        return;
      } else {
        throw Error::IoError(errno);
      }
    }
  }

  SocketCoro<void> TcpIpClient::recv(void* ptr, size_t size)
  {
    while (true) {
      const auto ret{::recv(socket_.raw(), ptr, size, 0)};

      if (ret < 0 && errno == EAGAIN) {
        co_await std::suspend_always{};
      } else if (static_cast<size_t>(ret) == size) {
        co_return;
      } else {
        throw Error::IoError(errno);
      }
    }
  }

  TcpIpServer::TcpIpServer(const std::string& ip, const std::string& port) : socket_(TcpIpSocket::server(ip, port)) {}

  SocketCoro<TcpIpClient> TcpIpServer::accept()
  {
    FileDescriptor fd = co_await sys_accept(socket_);
    setNonBlockingMode(fd);

    co_return TcpIpClient{TcpIpSocket{std::move(fd)}};
  }
} // namespace C8::Network
