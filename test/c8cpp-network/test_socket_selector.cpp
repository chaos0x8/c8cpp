#include "c8cpp-network/socket_selector.hpp"
#include "c8cpp-network/tcp_ip_socket.hpp"

#include <gmock/gmock.h>

#include <future>
#include <thread>

namespace C8::Network {
  using namespace testing;

  struct TestSocketSelector : public Test {
    TestSocketSelector()
    {
      cut.start();
    }

    std::string ip = "127.0.0.1";
    std::string port = "4242";
    SocketSelector cut{};
  };

  TEST_F(TestSocketSelector, sendReceivedStringDataBetweenSockets)
  {
    cut.insert(
      TcpIpServer{ip, port},
      [](TcpIpServer& server, SocketSelector& selector) -> SocketCoro<> {
        selector.insert(co_await server.accept(), [](TcpIpClient& client) -> SocketCoro<> {
          client.sendString("Hello world");
          co_return;
        });

        selector.insert(co_await server.accept(), [](TcpIpClient& client) -> SocketCoro<> {
          client.sendString("Bye world");
          co_return;
        });
      },
      std::ref(cut));

    std::promise<std::string> recv0Promise;
    std::future<std::string> recv0 = recv0Promise.get_future();

    std::promise<std::string> recv1Promise;
    std::future<std::string> recv1 = recv1Promise.get_future();

    cut.insert(
      TcpIpClient{ip, port},
      [](TcpIpClient& client, std::promise<std::string>& promise) -> SocketCoro<> {
        promise.set_value(co_await client.recvString());
      },
      std::move(recv0Promise));

    cut.insert(
      TcpIpClient{ip, port},
      [](TcpIpClient& client, std::promise<std::string>& promise) -> SocketCoro<> {
        promise.set_value(co_await client.recvString());
      },
      std::move(recv1Promise));

    EXPECT_THAT(recv0.get(), Eq("Hello world"));
    EXPECT_THAT(recv1.get(), Eq("Bye world"));
  }
} // namespace C8::Network
