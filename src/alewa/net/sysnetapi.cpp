#include "sysnetapi.hpp"

namespace alewa::net {

auto SysNetApi::error(int err) const -> std::string
{
    return strerror(err);
}

[[nodiscard]] int SysNetApi::err_no() const { return errno; }

SysNetApi::AIDeleter SysNetApi::freeaddrinfo [[maybe_unused]] = ::freeaddrinfo;

SysNetApi::SockCloser  SysNetApi::close [[maybe_unused]] = ::close;


}  // namespace alewa::net
