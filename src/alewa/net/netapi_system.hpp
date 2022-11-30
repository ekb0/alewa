#pragma once

#include <netdb.h>
#include <unistd.h>
#include <fcntl.h>

#include <string>

#include "utils.hpp"
#include "sys/err_desc.hpp"

namespace alewa::net {

struct SystemNetworkApi : public sys::SystemErrorDescription
{
    using AddrInfo = ::addrinfo;
    using AiDeleter = decltype(&::freeaddrinfo);
    using SockAddr = ::sockaddr;
    using SockLen = ::socklen_t;

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
