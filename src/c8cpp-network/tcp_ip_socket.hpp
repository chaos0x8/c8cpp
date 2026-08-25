#pragma once

#include "c8cpp-network/file_descriptor.hpp"
#include "c8cpp-network/socket_coro.hpp"

#include <span>
#include <string>
#include <vector>

namespace C8::Network {
  class TcpIpSocket {
  public:
    static TcpIpSocket server(const std::string& ip, const std::string& port);
    static TcpIpSocket connect(const std::string& ip, const std::string& port);

    explicit TcpIpSocket(FileDescriptor fd);
    TcpIpSocket(const TcpIpSocket&) = delete;
    TcpIpSocket(TcpIpSocket&& other);

    TcpIpSocket& operator=(const TcpIpSocket&) = delete;
    TcpIpSocket& operator=(TcpIpSocket&&);

    void reset();

    FileDescriptor::value_type raw() const;

  private:
    FileDescriptor fd_;
  };

  class TcpIpClient {
  public:
    explicit TcpIpClient(TcpIpSocket socket);
    explicit TcpIpClient(const std::string& ip, const std::string& port);

    SocketCoro<std::string> recvString();
    void sendString(std::string_view);

    SocketCoro<std::vector<std::byte>> recvBytes();
    void sendBytes(std::span<const std::byte>);

    void send(std::span<const std::byte>);
    SocketCoro<void> recv(std::span<std::byte>);

    void send(const void*, size_t);
    SocketCoro<void> recv(void*, size_t);

    FileDescriptor::value_type raw() const
    {
      return socket_.raw();
    }

  private:
    TcpIpSocket socket_;
  };

  class TcpIpServer {
  public:
    explicit TcpIpServer(const std::string& ip, const std::string& port);

    SocketCoro<TcpIpClient> accept();

    FileDescriptor::value_type raw() const
    {
      return socket_.raw();
    }

  private:
    TcpIpSocket socket_;
  };
} // namespace C8::Network
