#pragma once

#include <cstdint>
#include <filesystem>
#include <map>
#include <memory>
#include <span>
#include <string>
#include <vector>

namespace C8::Core {
  class OptionParser {
  public:
    struct HelpLine {
      std::string name;
      std::string placeholder;
      std::string help;
    };

    class Option {
    public:
      explicit Option(HelpLine helpLine, uint32_t priority) : helpLine_(std::move(helpLine)), priority_(priority) {}

      virtual ~Option() = default;

      std::string_view name() const {
        return helpLine_.name;
      }

      std::string_view placeholder() const {
        return helpLine_.placeholder;
      }

      std::string_view help() const {
        return helpLine_.help;
      }

      uint32_t priority() const {
        return priority_;
      }

      std::string format(const std::array<size_t, 2>& colSizes) const {
        std::stringstream ss;

        // clang-format off
        ss << std::setw(colSizes[0]) << std::left << helpLine_.name << " "
           << std::setw(colSizes[1]) << std::left << helpLine_.placeholder << " - "
           << helpLine_.help;
        // clang-format on

        return ss.str();
      }

      virtual uint32_t parse(std::span<const std::string> args) = 0;
      virtual void updateBanner(std::string& banner) = 0;

    private:
      HelpLine helpLine_;
      uint32_t priority_;
    };

    struct ParseState {
      bool parsed = false;
      bool bannerUpdated = false;
    };

    void regKeyword(std::string& out, std::string name, std::string placeholder, std::string help);

    void regOrdered(std::string placeholder, std::string help);
    void regOrdered(std::string& out, std::string placeholder, std::string help);
    void regOrdered(std::filesystem::path& out, std::string placeholder, std::string help);

    void regOption(bool& out, std::string name, std::string help);
    void regOption(std::string& out, std::string name, std::string placeholder, std::string help);
    void regOption(std::filesystem::path& out, std::string name, std::string placeholder, std::string help);

    std::vector<std::string> parse(std::span<const std::string> args);
    std::vector<std::string> parse(int argc, char** argv);
    std::string help() const;

  private:
    template <typename T, typename... Args>
    void regOptionT(Args&&...);

    std::vector<std::unique_ptr<Option>> options_;
    std::map<std::string, std::shared_ptr<ParseState>> keywordParseStates_;
    std::array<size_t, 2> colSizes_{};
  };
} // namespace C8::Core
