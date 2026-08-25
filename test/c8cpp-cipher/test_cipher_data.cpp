#include "c8cpp-cipher/cipher_data.hpp"

#include <gmock/gmock.h>

namespace C8::Cipher {
  using namespace testing;

  class TestCipherData : public Test {
  public:
    bool SpanEq(std::span<const std::byte> left, std::span<const std::byte> right) {
      return std::equal(left.begin(), left.end(), right.begin(), right.end());
    }
  };

  TEST_F(TestCipherData, constructFromStringView) {
    EXPECT_THAT(CipherDataRO("test").string_view(), Eq("test"));
  }

  TEST_F(TestCipherData, constructFromString) {
    std::string val = "str";

    EXPECT_THAT(CipherDataRO(val).string_view(), Eq("str"));
    EXPECT_THAT(CipherDataRW(val).string_view(), Eq("str"));
  }

  TEST_F(TestCipherData, constructFromSpan) {
    std::vector<std::byte> val = {{static_cast<std::byte>(42)}};

    EXPECT_THAT(SpanEq(val, CipherDataRO(val).span()), Eq(true));
    EXPECT_THAT(SpanEq(val, CipherDataRW(val).span()), Eq(true));
  }

  TEST_F(TestCipherData, returnSize) {
    std::string val = "str";

    EXPECT_THAT(CipherDataRO(val).size(), Eq(3));
    EXPECT_THAT(CipherDataRW(val).size(), Eq(3));
  }

  TEST_F(TestCipherData, returnU8DataPointer) {
    std::string val = "str";

    const uint8_t* cptr = CipherDataRO(val).asU8();
    EXPECT_THAT(cptr, Not(Eq(nullptr)));

    uint8_t* ptr = CipherDataRW(val).asU8();
    EXPECT_THAT(ptr, Not(Eq(nullptr)));
  }

  TEST_F(TestCipherData, returnCharDataPointer) {
    std::string val = "str";

    const char* cptr = CipherDataRO(val).asChar();
    EXPECT_THAT(cptr, Not(Eq(nullptr)));

    char* ptr = CipherDataRW(val).asChar();
    EXPECT_THAT(ptr, Not(Eq(nullptr)));
  }

  TEST_F(TestCipherData, returnByteDataPointer) {
    std::string val = "str";

    const std::byte* cptr = CipherDataRO(val).asByte();
    EXPECT_THAT(cptr, Not(Eq(nullptr)));

    std::byte* ptr = CipherDataRW(val).asByte();
    EXPECT_THAT(ptr, Not(Eq(nullptr)));
  }

  TEST_F(TestCipherData, returnSubview) {
    std::string val = "str";

    auto cview = CipherDataRO(val).subview(1, 1);
    EXPECT_THAT(cview.string_view(), Eq("t"));

    auto view = CipherDataRW(val).subview(1, 1);
    EXPECT_THAT(view.string_view(), Eq("t"));
  }

  TEST_F(TestCipherData, appendToCipherData) {
    CipherData out;
    CipherDataRO view{"abc"};
    out.append(view);

    EXPECT_THAT(out.view().string_view(), Eq("abc"));
  }
} // namespace C8::Cipher
