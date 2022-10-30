#include "sysnetapi.hpp"

namespace alewa::net {

auto SysNetApi::error() const -> char const *
{
    return strerror(errno);
}

SysNetApi::AIDeleter SysNetApi::freeaddrinfo [[maybe_unused]] = ::freeaddrinfo;

SysNetApi::SockCloser  SysNetApi::close [[maybe_unused]] = ::close;


}  // namespace alewa::net
