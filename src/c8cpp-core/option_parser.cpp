#include "c8cpp-core/option_parser.hpp"
#include "c8cpp-core/algorithm.hpp"

#include <algorithm>
#include <format>
#include <iomanip>
#include <sstream>
#include <string>

namespace C8::Core {
  using namespace std::string_literals;

  class StandardOption : public OptionParser::Option {
  public:
    StandardOption(std::string name, std::string placeholder, std::string help)
      : Option({std::move(name), std::move(placeholder), std::move(help)}, 30) {}

    void updateBanner(std::string& banner) override final {}
  };

  class BooleanOption : public StandardOption {
  public:
    BooleanOption(bool& out, std::string name, std::string placeholder, std::string help)
      : StandardOption(std::move(name), std::move(placeholder), std::move(help)), out_(out) {}

    uint32_t parse(std::span<const std::string> args) override {
      if (args.size() >= 1 && args[0] == name()) {
        out_ = true;
        return 1;
      }

      return 0;
    }

  private:
    bool& out_;
  };

  class StringOption : public StandardOption {
  public:
    StringOption(std::string& out, std::string name, std::string placeholder, std::string help)
      : StandardOption(std::move(name), std::move(placeholder), std::move(help)), out_(out) {}

    uint32_t parse(std::span<const std::string> args) override {
      if (args.size() >= 1 && args[0] == name()) {
        if (args.size() >= 2 && !args[1].starts_with("-")) {
          out_ = args[1];
          return 2;
        }
        return 1;
      }
      return 0;
    }

  private:
    std::string& out_;
  };

  class PathOption : public StandardOption {
  public:
    PathOption(std::filesystem::path& out, std::string name, std::string placeholder, std::string help)
      : StandardOption(std::move(name), std::move(placeholder), std::move(help)), out_(out) {}

  protected:
    uint32_t parse(std::span<const std::string> args) override {
      if (args.size() >= 1 && args[0] == name()) {
        if (args.size() >= 2 && !args[1].starts_with("-")) {
          out_ = std::filesystem::path(args[1]);
          if (std::filesystem::exists(out_)) {
            out_ = std::filesystem::canonical(out_);
          }
          return 2;
        }
        return 1;
      }
      return 0;
    }

  private:
    std::filesystem::path& out_;
  };

  class OrderedOption : public OptionParser::Option {
  public:
    OrderedOption(std::string name, std::string placeholder, std::string help,
      const std::shared_ptr<OptionParser::ParseState>& state)
      : Option({std::move(name), std::move(placeholder), std::move(help)}, 20), state_(state) {}

    uint32_t parse(std::span<const std::string> args) override final {
      if (state_->parsed) {
        return 0;
      }

      const auto ret{parseOrdered(args)};

      if (ret > 0) {
        state_->parsed = true;
      }

      return ret;
    }

    void updateBanner(std::string& banner) override final {
      if (state_->bannerUpdated) {
        return;
      }

      if (!placeholder().empty()) {
        state_->bannerUpdated = true;
        banner += std::format(" {}", placeholder());
      }
    }

  protected:
    virtual uint32_t parseOrdered(std::span<const std::string> args) = 0;

  private:
    std::shared_ptr<OptionParser::ParseState> state_;
  };

  class IgnoreOrderedOption : public OrderedOption {
  public:
    IgnoreOrderedOption(std::string placeholder, std::string help)
      : OrderedOption(""s, std::move(placeholder), std::move(help), std::make_shared<OptionParser::ParseState>()) {}

  protected:
    uint32_t parseOrdered(std::span<const std::string> args) override {
      if (args.size() >= 1 && !args[0].starts_with("-")) {
        return 1;
      }

      return 0;
    }
  };

  class StringOrderedOption : public OrderedOption {
  public:
    StringOrderedOption(std::string& out, std::string placeholder, std::string help)
      : OrderedOption(""s, std::move(placeholder), std::move(help), std::make_shared<OptionParser::ParseState>()),
        out_(out) {}

  protected:
    uint32_t parseOrdered(std::span<const std::string> args) override {
      if (args.size() >= 1 && !args[0].starts_with("-")) {
        out_ = args[0];
        return 1;
      }

      return 0;
    }

  private:
    std::string& out_;
  };

  class PathOrderedOption : public OrderedOption {
  public:
    PathOrderedOption(std::filesystem::path& out, std::string placeholder, std::string help)
      : OrderedOption(""s, std::move(placeholder), std::move(help), std::make_shared<OptionParser::ParseState>()),
        out_(out) {}

  protected:
    uint32_t parseOrdered(std::span<const std::string> args) override {
      if (args.size() >= 1 && !args[0].starts_with("-")) {
        out_ = std::filesystem::path(args[0]);
        if (std::filesystem::exists(out_)) {
          out_ = std::filesystem::canonical(out_);
        }
        return 1;
      }

      return 0;
    }

  private:
    std::filesystem::path& out_;
  };

  class StringKeywordOption : public OrderedOption {
  public:
    StringKeywordOption(std::string& out, std::string name, std::string placeholder, std::string help,
      const std::shared_ptr<OptionParser::ParseState>& parseState)
      : OrderedOption(std::move(name), std::move(placeholder), std::move(help), parseState), out_(out) {}

  protected:
    uint32_t parseOrdered(std::span<const std::string> args) override {
      if (args.size() >= 1 && args[0] == name()) {
        out_ = args[0];
        return 1;
      }

      return 0;
    }

  private:
    std::string& out_;
  };

  template <typename T, typename... Args>
  void OptionParser::regOptionT(Args&&... args) {
    auto option = std::make_unique<T>(std::forward<Args>(args)...);

    colSizes_[0] = std::max(colSizes_[0], option->name().size());
    colSizes_[1] = std::max(colSizes_[1], option->placeholder().size());

    options_.emplace_back(std::move(option));

    std::ranges::stable_sort(
      options_, [](const auto& left, const auto& right) { return left->priority() < right->priority(); });
  }

  void OptionParser::regOption(bool& out, std::string name, std::string help) {
    regOptionT<BooleanOption>(out, std::move(name), "", std::move(help));
  }

  void OptionParser::regOption(std::string& out, std::string name, std::string placeholder, std::string help) {
    regOptionT<StringOption>(out, std::move(name), std::move(placeholder), std::move(help));
  }

  void OptionParser::regOption(
    std::filesystem::path& out, std::string name, std::string placeholder, std::string help) {
    regOptionT<PathOption>(out, std::move(name), std::move(placeholder), std::move(help));
  }

  void OptionParser::regOrdered(std::string placeholder, std::string help) {
    regOptionT<IgnoreOrderedOption>(std::move(placeholder), std::move(help));
  }

  void OptionParser::regOrdered(std::string& out, std::string placeholder, std::string help) {
    regOptionT<StringOrderedOption>(out, std::move(placeholder), std::move(help));
  }

  void OptionParser::regOrdered(std::filesystem::path& out, std::string placeholder, std::string help) {
    regOptionT<PathOrderedOption>(out, std::move(placeholder), std::move(help));
  }

  void OptionParser::regKeyword(std::string& out, std::string name, std::string placeholder, std::string help) {
    if (keywordParseStates_.find(placeholder) == keywordParseStates_.end()) {
      keywordParseStates_[placeholder] = std::make_shared<ParseState>();
    }

    regOptionT<StringKeywordOption>(
      out, std::move(name), std::move(placeholder), std::move(help), keywordParseStates_[placeholder]);
  }

  std::vector<std::string> OptionParser::parse(std::span<const std::string> args) {
    std::vector<std::string> out;

    while (!args.empty()) {
      size_t usedArgs = 0;

      for (auto& option : options_) {
        usedArgs += option->parse(args);
        if (usedArgs > 0) {
          args = args.subspan(usedArgs);
          break;
        }
      }

      if (!args.empty() && usedArgs == 0) {
        out.emplace_back(args[0]);
        args = args.subspan(1);
      }
    }

    return out;
  }

  std::vector<std::string> OptionParser::parse(int argc, char** argv) {
    std::vector<std::string> tmp;

    for (int i = 0; i < argc; ++i) {
      tmp.emplace_back(argv[i]);
    }

    return parse(tmp);
  }

  std::string OptionParser::help() const {
    std::stringstream ss;

    std::string banner;

    for (const auto& option : options_) {
      option->updateBanner(banner);

      if (option->help().empty()) {
        continue;
      }

      ss << option->format(colSizes_) << "\n";
    }

    return std::format("Usage: program{} [options]\n\n{}", banner, ss.str());
  }
} // namespace C8::Core
