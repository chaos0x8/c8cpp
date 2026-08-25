#include "c8cpp-cipher/global_init.hpp"

#include <gmock/gmock.h>

namespace C8::Cipher {
  using namespace testing;

  TEST(TestGlobalInit, callsGlobalInitialize) {
    GlobalInit init0;
    GlobalInit init1;
  }
} // namespace C8::Cipher
