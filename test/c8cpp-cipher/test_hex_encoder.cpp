#include "c8cpp-cipher/hex_encoder.hpp"
#include "c8cpp-cipher/utility.hpp"

#include <gmock/gmock.h>

namespace C8::Cipher {
  using namespace testing;
  using Utility::toString;

  class TestHexEncoder : public Test {
  public:
    static constexpr std::string_view data = "hello";
  };

  TEST_F(TestHexEncoder, inlineEncodesData) {
    EXPECT_THAT(HexEncoder::encode(data), Eq("68656c6c6f"));
  }

  TEST_F(TestHexEncoder, inlineDecodesStringData) {
    const auto encoded = HexEncoder::encode(data);
    EXPECT_THAT(HexEncoder::decode(encoded).view(), Eq(CipherDataRO{data}));
  }

  class TestHexEncoderIO : public TestHexEncoder {
  public:
    HexEncoder::IO cut;
  };

  TEST_F(TestHexEncoderIO, encodesData) {
    cut << data;
    EXPECT_THAT(cut.hex(), Eq("68656c6c6f"));
  }

  TEST_F(TestHexEncoderIO, decodesData) {
    cut << data;
    std::string out1(2, '\0');
    std::string out2(3, '\0');
    CipherDataRW view1{out1};
    CipherDataRW view2{out2};
    cut >> view1 >> view2;
    EXPECT_THAT(out1, Eq("he"));
    EXPECT_THAT(out2, Eq("llo"));
  }

  TEST_F(TestHexEncoderIO, decodesRemainingData) {
    cut << data;
    CipherData out;
    cut >> out;
    EXPECT_THAT(out.view().string_view(), Eq("hello"));
  }
} // namespace C8::Cipher
