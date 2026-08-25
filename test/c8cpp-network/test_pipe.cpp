#include "c8cpp-network/error.hpp"
#include "c8cpp-network/pipe.hpp"

#include <gmock/gmock.h>

namespace C8::Network {
  using namespace testing;

  class TestPipe : public Test {
  public:
    template <typename T>
    T resolve(SocketCoro<T> coro) {
      while (!coro.done()) {
        coro.resume();
      }

      return coro.value();
    }

    Pipe cut = createPipe();
  };

  TEST_F(TestPipe, readsWrittenData) {
    cut.w.writeString("abcdefg");

    EXPECT_THAT(resolve(cut.r.readString()), Eq("abcdefg"));
  }

  TEST_F(TestPipe, readsOnlyPortionOfData) {
    std::string data(128 + 64, 'a');
    cut.w.writeString(data);

    EXPECT_THAT(resolve(cut.r.readString()), Eq(std::string(128, 'a')));
    EXPECT_THAT(resolve(cut.r.readString()), Eq(std::string(64, 'a')));
  }
} // namespace C8::Network
