#include "c8cpp-core/rpc_engine.hpp"

#include "c8cpp-core/error.hpp"

#include <gmock/gmock.h>

#include <stdexcept>

namespace C8::Core {
  using namespace testing;
  using namespace std::string_literals;

  class TestRpcEngine : public Test {
  public:
    using HelloWorld = RpcFunction<std::define_static_string("hello_world"), int8_t(int8_t, int8_t)>;
    using HelloWorld2 = RpcFunction<std::define_static_string("hello_world_2"), int8_t(int8_t, int8_t)>;
    using Concat = RpcFunction<std::define_static_string("concat"), std::string(std::string, std::string)>;
    using Join = RpcFunction<std::define_static_string("join"), std::string(std::vector<std::string>, std::string)>;
    using Thrower = RpcFunction<std::define_static_string("thrower"), void(std::string)>;

    void SetUp() override
    {
      sut.registerHandler<HelloWorld>([](int8_t a, int8_t b) { return a + b; });

      sut.registerHandler<HelloWorld2>([](auto a, auto b) {
        static_assert(std::is_same_v<decltype(a), int8_t>);
        static_assert(std::is_same_v<decltype(b), int8_t>);
        return a + b;
      });

      sut.registerHandler<Concat>(
        [](std::string_view a, std::string_view b) -> std::string { return std::string(a) + b; });

      sut.registerHandler<Join>([](std::span<const std::string> a, std::string_view b) -> std::string {
        return a | std::ranges::views::join_with(b) | std::ranges::to<std::string>();
      });

      sut.registerHandler<Thrower>([](std::string&& what) { throw std::runtime_error(std::move(what)); });
    }

    template <typename Func, typename F, typename... Args>
    auto performCall(F&& func, Args&&... args)
    {
      return Func::decodeReturn(Func::decodeCall(Func::encodeCall(std::forward<Args>(args)...), std::forward<F>(func)));
    }

    RpcEngine sut;
  };

  TEST_F(TestRpcEngine, rcpFunction_decodesEncodedCallPayload_1)
  {
    auto result = performCall<HelloWorld>([](auto a, auto b) { return a + b; }, 7, 14);
    EXPECT_THAT(result.value(), Eq(21));
  }

  TEST_F(TestRpcEngine, rcpFunction_decodesEncodedCallPayload_2)
  {
    auto result = performCall<Concat>([](auto a, auto b) { return a + b; }, "tyg", "rys");
    EXPECT_THAT(result.value(), StrEq("tygrys"));
  }

  TEST_F(TestRpcEngine, rpcFunction_throwsWhenEncodedCallPayloadIsInvalid_1)
  {
    auto payload = HelloWorld::encodeCall(7, 14);
    EXPECT_THROW((HelloWorld2::decodeCall(payload, [](auto a, auto b) { return a + b; })), Error::AssertError);
  }

  TEST_F(TestRpcEngine, rpcFunction_throwsWhenEncodedCallPayloadIsInvalid_2)
  {
    auto payload = Concat::encodeCall("tyg", "rys");
    EXPECT_THROW((HelloWorld::decodeCall(payload, [](auto a, auto b) { return a + b; })), Error::AssertError);
  }

  TEST_F(TestRpcEngine, rpcFunction_throwsWhenEncodedCallPayloadIsInvalid_3)
  {
    auto payload = Concat::encodeCall("tyg", "rys");
    payload.pop_back();
    EXPECT_THROW((Concat::decodeCall(payload, [](auto a, auto b) { return a + b; })), Error::AssertError);
  }

  TEST_F(TestRpcEngine, rpcFunction_throwsWhenEncodedReturnPayloadIsInvalid_1)
  {
    auto payload = HelloWorld::decodeCall(HelloWorld::encodeCall(0, 0), [](auto, auto) { return 4; });
    EXPECT_THROW((void)HelloWorld2::decodeReturn(payload), Error::AssertError);
  }

  TEST_F(TestRpcEngine, rpcFunction_throwsWhenEncodedReturnPayloadIsInvalid_2)
  {
    auto payload = Concat::decodeCall(Concat::encodeCall("", ""), [](auto, auto) { return "ocelot"; });
    EXPECT_THROW((void)HelloWorld::decodeReturn(payload), Error::AssertError);
  }

  TEST_F(TestRpcEngine, rpcFunction_throwsWhenEncodedReturnPayloadIsInvalid_3)
  {
    auto payload = Concat::decodeCall(Concat::encodeCall("", ""), [](auto, auto) { return "ocelot"; });
    payload.pop_back();
    EXPECT_THROW((void)Concat::decodeReturn(payload), Error::AssertError);
  }

  TEST_F(TestRpcEngine, rpcFunction_throwsWhenEncodedReturnPayloadIsInvalid_4)
  {
    auto payload = Concat::encodeCall("oce", "lot");
    EXPECT_THROW((void)Concat::decodeReturn(payload), Error::AssertError);
  }

  TEST_F(TestRpcEngine, rpcFunction_returnsExceptionWhatWhenDecodeThrows_1)
  {
    auto result =
      performCall<Thrower>([](std::string&& what) { throw std::runtime_error(std::move(what)); }, "error msg 1");
    EXPECT_THAT(result.error(), StrEq("error msg 1"));
  }

  TEST_F(TestRpcEngine, rpcFunction_returnsExceptionWhatWhenDecodeThrows_2)
  {
    auto result =
      performCall<Thrower>([](std::string&& what) { throw std::runtime_error(std::move(what)); }, "error msg 2");
    EXPECT_THAT(result.error(), StrEq("error msg 2"));
  }

  TEST_F(TestRpcEngine, routesPayloadToCorrectHandler_1)
  {
    auto result = HelloWorld::decodeReturn(sut.dispatchCall(HelloWorld::encodeCall(3, 39)));
    EXPECT_THAT(result.value(), Eq(42));
  }

  TEST_F(TestRpcEngine, routesPayloadToCorrectHandler_2)
  {
    auto result = Join::decodeReturn(sut.dispatchCall(Join::encodeCall({"Hello", "World"}, " ")));
    EXPECT_THAT(result.value(), Eq("Hello World"));
  }

  TEST_F(TestRpcEngine, routesPayloadToCorrectHandler_3)
  {
    auto result = Thrower::decodeReturn(sut.dispatchCall(Thrower::encodeCall("tygrys")));
    EXPECT_THAT(result.error(), Eq("tygrys"));
  }

  TEST_F(TestRpcEngine, throwsWhenTryingToRegisterDuplicatedHandler)
  {
    EXPECT_THROW(sut.registerHandler<HelloWorld>([](auto, auto) { return 0; }), Error::AssertError);
  }

  TEST_F(TestRpcEngine, supportsCustomTypes)
  {
    struct InputType {
      std::string value;
    };

    struct OutputType {
      int16_t value;
    };

    using CustomHandler = RpcFunction<std::define_static_string("custom_handler"), OutputType(InputType)>;

    sut.registerHandler<CustomHandler>(
      [](const InputType& input) { return OutputType{.value = static_cast<int16_t>(input.value.size())}; });

    auto result =
      CustomHandler::decodeReturn(sut.dispatchCall(CustomHandler::encodeCall(InputType{.value = "tygrys"})));
    EXPECT_THAT(result.value().value, Eq(6));
  }

  TEST_F(TestRpcEngine, supportsVoidHandler)
  {
    using CustomHandler = RpcFunction<std::define_static_string("custom_handler"), void()>;

    bool called = false;
    sut.registerHandler<CustomHandler>([&called]() { called = true; });

    auto result = CustomHandler::decodeReturn(sut.dispatchCall(CustomHandler::encodeCall()));
    EXPECT_NO_THROW(result.value());
    EXPECT_THAT(called, Eq(true));
  }
} // namespace C8::Core
