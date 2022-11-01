#include "sysnetapi.hpp"

namespace alewa::net {

auto SysNetApi::error(int err) const -> std::string { return strerror(err); }

auto SysNetApi::err_no() const -> int { return errno; }

}  // namespace alewa::net
