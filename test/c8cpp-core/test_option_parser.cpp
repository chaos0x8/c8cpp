#include "c8cpp-core/option_parser.hpp"

#include <string>

#include <gmock/gmock.h>

namespace C8::Core {
  using namespace testing;
  using namespace std::string_literals;

  class TestOptionParser : public Test {
  public:
    std::string app = "path/app";

    OptionParser cut;
  };

  TEST_F(TestOptionParser, parse_optionBoolean_parsedAsTrue) {
    bool verbose = false;
    bool somethingElse = false;

    cut.regOption(verbose, "--verbose", "Prints more information");
    cut.regOption(somethingElse, "--something-else", "Something else");

    const auto args = cut.parse({{"--verbose"s, "--unknown"s}});

    EXPECT_THAT(verbose, Eq(true));
    EXPECT_THAT(somethingElse, Eq(false));
    EXPECT_THAT(args, ElementsAre("--unknown"));
  }

  TEST_F(TestOptionParser, parse_optionString_parsedAsOriginal) {
    std::string arg;

    cut.regOption(arg, "--name", "NAME", "Some help");

    const auto args = cut.parse({{"--name"s, "kevin"s}});

    EXPECT_THAT(arg, Eq("kevin"));
    EXPECT_THAT(args, ElementsAre());
  }

  TEST_F(TestOptionParser, parse_orderedString_parsedAsOriginal) {
    std::string arg;

    cut.regOrdered(arg, "ARG", "Some help");

    const auto args = cut.parse({{"Kevin"s}});

    EXPECT_THAT(arg, Eq("Kevin"));
    EXPECT_THAT(args, ElementsAre());
  }

  TEST_F(TestOptionParser, parse_optionPath_parsedAsCanonicalWhenExists) {
    std::filesystem::path arg;

    cut.regOption(arg, "--dir", "PATH", "Some help");

    const auto input = std::filesystem::temp_directory_path() / ".";
    const auto args = cut.parse({{"--dir"s, input.string()}});

    EXPECT_THAT(arg.string(), Eq(std::filesystem::temp_directory_path().string()));
    EXPECT_THAT(args, ElementsAre());
  }

  TEST_F(TestOptionParser, parse_optionPath_parsedAsOriginalWhenDoesNotExist) {
    std::filesystem::path arg;

    cut.regOption(arg, "--dir", "PATH", "Some help");

    const auto input = std::filesystem::temp_directory_path() / "abc" / ".";
    const auto args = cut.parse({{"--dir"s, input.string()}});

    EXPECT_THAT(arg.string(), Eq((std::filesystem::temp_directory_path() / "abc" / ".").string()));
    EXPECT_THAT(args, ElementsAre());
  }

  TEST_F(TestOptionParser, parse_orderedPath_parsedAsCanonicalWhenExists) {
    std::filesystem::path arg;

    cut.regOrdered(arg, "PATH", "Some help");

    const auto input = std::filesystem::temp_directory_path() / ".";
    const auto args = cut.parse({{input.string()}});

    EXPECT_THAT(arg.string(), Eq(std::filesystem::temp_directory_path().string()));
    EXPECT_THAT(args, ElementsAre());
  }

  TEST_F(TestOptionParser, parse_orderedPath_parsedAsOriginalWhenDoesNotExist) {
    std::filesystem::path arg;

    cut.regOrdered(arg, "PATH", "Some help");

    const auto input = std::filesystem::temp_directory_path() / "abc" / ".";
    const auto args = cut.parse({{input.string()}});

    EXPECT_THAT(arg.string(), Eq((std::filesystem::temp_directory_path() / "abc" / ".").string()));
    EXPECT_THAT(args, ElementsAre());
  }

  TEST_F(TestOptionParser, parse_option_ArgumentMustBeAfterOptionToBeParsed) {
    std::string arg0;
    std::string arg1;

    cut.regOption(arg0, "--one", "ONE", "Help1");
    cut.regOption(arg1, "--two", "TWO", "Help2");

    const auto args = cut.parse({{"--one", "--two", "val1", "val2"}});

    EXPECT_THAT(arg0, Eq(""));
    EXPECT_THAT(arg1, Eq("val1"));
    EXPECT_THAT(args, ElementsAre("val2"));
  }

  TEST_F(TestOptionParser, parse_keyword_lastKeywordIsUsed) {
    std::string arg0;

    cut.regKeyword(arg0, "add-item", "COMMAND", "Adds item to list");
    cut.regKeyword(arg0, "del-item", "COMMAND", "Removes item from list");

    const auto args = cut.parse({{"abc"s, "del-item"s, "add-item"s}});

    EXPECT_THAT(arg0, Eq("del-item"));
    EXPECT_THAT(args, ElementsAre("abc", "add-item"));
  }

  TEST_F(TestOptionParser, parse_parsesMixedOptions) {
    std::string arg0;
    bool arg1;
    std::string arg2;
    bool arg3;

    cut.regOption(arg0, "--name", "NAME", "Some help 0");
    cut.regOption(arg1, "--switch", "Some help 1");
    cut.regOrdered(arg2, "ALIAS", "Some help 2");
    cut.regOption(arg3, "--missing", "Some help 3");

    const auto args =
      cut.parse({{"Destroyer"s, "--name", "Kevin", "--switch", "Something", "--unexpected", "Unexpected"}});

    EXPECT_THAT(arg0, Eq("Kevin"));
    EXPECT_THAT(arg1, Eq(true));
    EXPECT_THAT(arg2, Eq("Destroyer"));
    EXPECT_THAT(arg3, Eq(false));
    EXPECT_THAT(args, ElementsAre("Something", "--unexpected", "Unexpected"));
  }

  TEST_F(TestOptionParser, help_skipsOptionsWithEmptyHelp) {
    std::string arg0;
    std::string arg1;

    cut.regOption(arg0, "--arg0", "TXT", "");
    cut.regOption(arg1, "--arg1", "TXT", "Some help 1");

    // clang-format off
    EXPECT_THAT(cut.help(), Eq(
      "Usage: program [options]\n" +
      "\n"s +
      "--arg1 TXT - Some help 1\n"s
    ));
    // clang-format on
  }

  TEST_F(TestOptionParser, help_returnsHelpForAllOptions) {
    bool arg0 = false;
    std::string arg1;
    std::string arg2;
    std::string arg3;
    std::string arg4;

    cut.regOption(arg0, "--bool", "Some help 0");
    cut.regOrdered(arg1, "NAME", "Some help 2");
    cut.regOption(arg2, "--string", "TXT", "Some help 1");
    cut.regKeyword(arg3, "add-item", "COMMAND", "Adds item to list");
    cut.regKeyword(arg3, "delete-item", "COMMAND", "Removes item from list");
    cut.regOrdered(arg4, "LASTNAME", "Some help");

    // clang-format off
    EXPECT_THAT(cut.help(), Eq(
      "Usage: program NAME COMMAND LASTNAME [options]\n" +
      "\n"s +
      "            NAME     - Some help 2\n"s +
      "add-item    COMMAND  - Adds item to list\n"s +
      "delete-item COMMAND  - Removes item from list\n"s +
      "            LASTNAME - Some help\n"s +
      "--bool               - Some help 0\n"s +
      "--string    TXT      - Some help 1\n"s
    ));
    // clang-format on
  }
} // namespace C8::Core
