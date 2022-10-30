#pragma once

#include <cerrno>
#include <cstring>
#include <netdb.h>
#include <unistd.h>

namespace alewa::net {

struct SysNetApi
{
    #define SYSNET_DELEGATE(func, delegate)                                    \
            decltype(&(delegate)) const func = (delegate)

    /* ProviderBase */
    using AddrInfo = ::addrinfo;

    static int const ERROR = -1;
    static int const SUCCESS = 0;

    [[nodiscard]] auto error() const -> char const * ;

    /* AddrInfoProvider */
    using AIDeleter [[maybe_unused]] = decltype(&::freeaddrinfo);

    static AIDeleter freeaddrinfo;
    SYSNET_DELEGATE(getaddrinfo, ::getaddrinfo);
    SYSNET_DELEGATE(gai_strerror, ::gai_strerror);

    /* SocketProvider */
    using SockAddr [[maybe_unused]] = ::sockaddr;
    using SockLen_t [[maybe_unused]] = ::socklen_t;
    using SockCloser [[maybe_unused]] = decltype(&::close);

    static SockCloser close;
    SYSNET_DELEGATE(socket, ::socket);
    SYSNET_DELEGATE(bind, ::bind);
    SYSNET_DELEGATE(connect, ::connect);
};

}  // namespace alewa::net
