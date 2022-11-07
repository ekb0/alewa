#include "netapi_system.hpp"

namespace alewa::net {

auto SystemNetworkApi::error(int err) const -> std::string { return strerror(err); }

auto SystemNetworkApi::err_no() const -> int { return errno; }

}  // namespace alewa::net
