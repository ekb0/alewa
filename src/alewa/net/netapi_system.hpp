#pragma once

#include <netdb.h>
#include <unistd.h>
#include <fcntl.h>

#include <string>

#include "sys/err_desc.hpp"

namespace alewa::net {

struct SystemNetworkApi : public sys::SystemErrorDescription
{
    using AddrInfo = ::addrinfo;
    using AiDeleter = decltype(&::freeaddrinfo);
    using SockAddr = ::sockaddr;
    using SockLen = ::socklen_t;

    [[nodiscard]]
    auto getaddrinfo(char const * node, char const * service,
                     AddrInfo const * hints, AddrInfo** p_ai_list) const -> int
    {
        return ::getaddrinfo(node, service, hints, p_ai_list);
    }

    AiDeleter const freeaddrinfo = ::freeaddrinfo;

    [[nodiscard]]
    auto gai_strerror(int errcode) const -> char const *
    {
        return ::gai_strerror(errcode);
    }

    [[nodiscard]]
    auto socket(int domain, int type, int protocol) const -> int
    {
        return ::socket(domain, type, protocol);
    }

    auto close(int sockfd) const -> int { return ::close(sockfd); }

    [[nodiscard]]
    auto bind(int sockfd, SockAddr const * addr, SockLen addrlen) const -> int
    {
        return ::bind(sockfd, addr, addrlen);
    }

    [[nodiscard]]
    auto connect(int sockfd, SockAddr const * addr, SockLen addrlen) const
            -> int
    {
        return ::connect(sockfd, addr, addrlen);
    }

    [[nodiscard]]
    auto listen(int sockfd, int backlog) const -> int
    {
        return ::listen(sockfd, backlog);
    }


    [[nodiscard]]
    auto accept(int sockfd, SockAddr* recv_addr, SockLen* recv_addrlen) const
            -> int
    {
        return ::accept(sockfd, recv_addr, recv_addrlen);
    }

    [[nodiscard]]
    auto setsockopt(int sockfd, int level, int optname, void const * optval,
                      SockLen optlen) const -> int
    {
        return ::setsockopt(sockfd, level, optname, optval, optlen);
    }

    [[nodiscard]]
    auto fcntl(int sockfd, int cmd, int arg) const -> int
    {
        return ::fcntl(sockfd, cmd, arg);
    }
};

}  // namespace alewa::net
