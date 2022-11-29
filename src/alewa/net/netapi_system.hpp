#pragma once

#include <cerrno>
#include <cstring>
#include <netdb.h>
#include <unistd.h>
#include <fcntl.h>

#include <string>

#include "utils.hpp"

namespace alewa::net {

struct SystemNetworkApi
{
    using AddrInfo = ::addrinfo;
    using AiDeleter = decltype(&::freeaddrinfo);
    using SockAddr = ::sockaddr;
    using SockLen_t = ::socklen_t;

    static int const ERROR = -1;
    static int const SUCCESS = 0;

    [[nodiscard]] std::string error() const;

    ALW_DELEGATE(getaddrinfo, ::getaddrinfo);
    ALW_DELEGATE(freeaddrinfo, ::freeaddrinfo);
    ALW_DELEGATE(gai_strerror, ::gai_strerror);

    ALW_DELEGATE(socket, ::socket);
    ALW_DELEGATE(close, ::close);
    ALW_DELEGATE(bind, ::bind);
    ALW_DELEGATE(connect, ::connect);
    ALW_DELEGATE(listen, ::listen);
    ALW_DELEGATE(accept, ::accept);
    ALW_DELEGATE(setsockopt, ::setsockopt);

    ALW_DELEGATE(fcntl, ::fcntl);
};

}  // namespace alewa::net
