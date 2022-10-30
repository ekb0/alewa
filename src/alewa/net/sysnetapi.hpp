#pragma once

#include <cerrno>
#include <cstring>
#include <netdb.h>
#include <unistd.h>

namespace alewa::net {

struct SysNetApi
{
    /* types */
    using AddrInfo = ::addrinfo;
    using AIDeleter [[maybe_unused]] = decltype(&::freeaddrinfo);

    using SockAddr [[maybe_unused]] = ::sockaddr;
    using SockLen_t [[maybe_unused]] = ::socklen_t;
    using SockCloser [[maybe_unused]] = decltype(&::close);

    /* constants */
    static int const ERROR = -1;
    static int const SUCCESS = 0;

    /* methods */
    [[nodiscard]] auto neterror() const -> char const * ;

    #define SYSNET_DELEGATE(func, delegate)                                    \
        decltype(&(delegate)) const func = (delegate)

    SYSNET_DELEGATE(getaddrinfo, ::getaddrinfo);
    SYSNET_DELEGATE(freeaddrinfo, ::freeaddrinfo);
    SYSNET_DELEGATE(gai_strerror, ::gai_strerror);
    SYSNET_DELEGATE(socket, ::socket);
    SYSNET_DELEGATE(close, ::close);
    SYSNET_DELEGATE(bind, ::bind);
    SYSNET_DELEGATE(connect, ::connect);
};

}  // namespace alewa::net
