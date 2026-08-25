#include "c8cpp-core/rpc_engine.hpp"

namespace C8::Core {
  std::vector<std::byte> RpcEngine::dispatchCall(std::span<const std::byte> payload)
  {
    std::string signature;
    decode(signature, payload);

    auto it = map_.find(signature);
    C8_ASSERT(it != map_.end(), "Cound not find handler for \"{}\"", signature);
    C8_TRACE("RpcEngine dispatch {}", signature);

    return it->second(payload);
  }
} // namespace C8::Core
