#include "c8cpp-network/tcp_ip_socket.hpp"

#include <gmock/gmock.h>

#include <thread>

namespace C8::Network {
  using namespace testing;

  struct TestTcpIpSocket : public Test {
    template <typename T>
    static T resolve(SocketCoro<T> coro)
    {
      while (!coro.done()) {
        coro.resume();
      }

      return coro.value();
    }

    std::string ip = "127.0.0.1";
    std::string port = "4242";
    TcpIpServer server{ip, port};
  };

  TEST_F(TestTcpIpSocket, sendReceivedStringDataBetweenSockets)
  {
    auto t = std::thread([this] {
      auto client0 = resolve(server.accept());
      client0.sendString("Hello world");

      auto client1 = resolve(server.accept());
      client1.sendString("Bye world");
    });

    TcpIpClient client0{ip, port};
    EXPECT_THAT(resolve(client0.recvString()), Eq("Hello world"));

    TcpIpClient client1{ip, port};
    EXPECT_THAT(resolve(client1.recvString()), Eq("Bye world"));

    t.join();
  }

  TEST_F(TestTcpIpSocket, sendReceivedBytesDataBetweenSockets)
  {
    auto t = std::thread([this] {
      auto client0 = resolve(server.accept());
      client0.sendBytes(std::as_bytes(std::span{"Hello world"}));

      auto client1 = resolve(server.accept());
      client1.sendBytes(std::as_bytes(std::span{"Bye world"}));
    });

    TcpIpClient client0{ip, port};
    EXPECT_THAT(std::ranges::equal(resolve(client0.recvBytes()), std::as_bytes(std::span{"Hello world"})), Eq(true));

    TcpIpClient client1{ip, port};
    EXPECT_THAT(std::ranges::equal(resolve(client1.recvBytes()), std::as_bytes(std::span{"Bye world"})), Eq(true));

    t.join();
  }
} // namespace C8::Network
