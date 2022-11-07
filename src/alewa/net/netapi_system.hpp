#pragma once

#include <cerrno>
#include <cstring>
#include <netdb.h>
#include <unistd.h>
#include <string>

namespace alewa::net {

struct SystemNetworkApi
{
    #define NETAPI_DELEGATE(func, delegate)                                    \
            decltype(&(delegate)) const func = (delegate)

    using AddrInfo = ::addrinfo;
    using AiDeleter = decltype(&::freeaddrinfo);
    using SockAddr = ::sockaddr;
    using SockLen_t = ::socklen_t;

    static int const ERROR = -1;
    static int const SUCCESS = 0;

    [[nodiscard]] auto error(int err) const -> std::string;
    [[nodiscard]] auto err_no() const -> int;

    NETAPI_DELEGATE(getaddrinfo, ::getaddrinfo);
    NETAPI_DELEGATE(freeaddrinfo, ::freeaddrinfo);
    NETAPI_DELEGATE(gai_error, ::gai_strerror);

    NETAPI_DELEGATE(socket, ::socket);
    NETAPI_DELEGATE(close, ::close);
    NETAPI_DELEGATE(bind, ::bind);
    NETAPI_DELEGATE(connect, ::connect);
    NETAPI_DELEGATE(listen, ::listen);
    NETAPI_DELEGATE(accept, ::accept);
};

}  // namespace alewa::net
