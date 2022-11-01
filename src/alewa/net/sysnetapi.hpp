#pragma once

#include <cerrno>
#include <cstring>
#include <netdb.h>
#include <unistd.h>
#include <string>

namespace alewa::net {

struct SysNetApi
{
    #define SYSNET_DELEGATE(func, delegate)                                    \
            decltype(&(delegate)) const func = (delegate)

    /* ProviderBase */
    using AddrInfo = ::addrinfo;

    static int const ERROR = -1;
    static int const SUCCESS = 0;

    [[nodiscard]] auto error(int err) const -> std::string;
    [[nodiscard]] auto err_no() const -> int;

    /* AddrInfoProvider */
    using AIDeleter [[maybe_unused]] = decltype(&::freeaddrinfo);

    SYSNET_DELEGATE(getaddrinfo, ::getaddrinfo);
    SYSNET_DELEGATE(freeaddrinfo, ::freeaddrinfo);
    SYSNET_DELEGATE(gai_error, ::gai_strerror);

    /* SocketProvider */
    using SockAddr [[maybe_unused]] = ::sockaddr;
    using SockLen_t [[maybe_unused]] = ::socklen_t;

    SYSNET_DELEGATE(socket, ::socket);
    SYSNET_DELEGATE(close, ::close);
    SYSNET_DELEGATE(bind, ::bind);
    SYSNET_DELEGATE(connect, ::connect);
    SYSNET_DELEGATE(listen, ::listen);
    SYSNET_DELEGATE(accept, ::accept);
};

}  // namespace alewa::net
