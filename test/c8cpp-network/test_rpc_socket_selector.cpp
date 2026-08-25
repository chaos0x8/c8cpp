#include "c8cpp-core/rpc_engine.hpp"
#include "c8cpp-network/error.hpp"
#include "c8cpp-network/socket_selector.hpp"
#include "c8cpp-network/tcp_ip_socket.hpp"

#include <gmock/gmock.h>

#include <future>
#include <iostream>
#include <thread>

namespace C8::Network {
  using namespace testing;

  using HelloWorld = Core::RpcFunction<std::define_static_string("helloWorld"), std::string(std::string)>;

  struct TestRcpSocketSelector : public Test {
    void setupEchoServer()
    {
      cut->insert(
        TcpIpServer{ip, port},
        [](TcpIpServer& server, SocketSelector* selector) -> SocketCoro<> {
          while (true) {
            selector->insert(co_await server.accept(), [](TcpIpClient& client) -> SocketCoro<> {
              Core::RpcEngine rpc;

              rpc.registerHandler<HelloWorld>([](const std::string& in) { return in + " World"; });

              while (true) {
                client.sendBytes(rpc.dispatchCall(co_await client.recvBytes()));
              }
            });
          }
        },
        cut.get());
    }

    template <typename T>
    T resolve(SocketCoro<T> coro)
    {
      while (!coro.done()) {
        coro.resume();
      }

      return coro.value();
    }

    std::unique_ptr<SocketSelector> cut = std::make_unique<SocketSelector>();
    std::string ip = "127.0.0.1";
    std::string port = "4243";
  };

  TEST_F(TestRcpSocketSelector, selectsSocketsForAcceptReceive)
  {
    cut->start();
    setupEchoServer();

    TcpIpClient client1{ip, port};
    TcpIpClient client2{ip, port};

    client1.sendBytes(HelloWorld::encodeCall("Hello"));
    client2.sendBytes(HelloWorld::encodeCall("World"));

    EXPECT_THAT(HelloWorld::decodeReturn(resolve(client1.recvBytes())).value(), Eq("Hello World"));
    EXPECT_THAT(HelloWorld::decodeReturn(resolve(client2.recvBytes())).value(), Eq("World World"));
  }
} // namespace C8::Network
