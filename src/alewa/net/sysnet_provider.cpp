#include "sysnet_provider.hpp"

namespace alewa::net {

auto SystemNetworkProvider::error(int err) const -> std::string { return strerror(err); }

auto SystemNetworkProvider::err_no() const -> int { return errno; }

}  // namespace alewa::net
