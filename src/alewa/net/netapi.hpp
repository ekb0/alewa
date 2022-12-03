#pragma once

#include <concepts>
#include "sys/err_desc.hpp"

namespace alewa::net {

template <typename T>
concept PosixNetworkApi = requires(T t)
{
    requires alewa::sys::ErrorDescription<T>;

    typename T::AddrInfo;
    typename T::AiDeleter;
    typename T::SockAddr;
    typename T::SockLen;

    requires requires(char const * node, char const * service,
                      typename T::AddrInfo const * hints,
                      typename T::AddrInfo** p_ai_list,
                      typename T::AddrInfo* ai_list, int errcode)
    {
        { t.getaddrinfo(node, service, hints, p_ai_list) }
                -> std::same_as<int>;
        { t.freeaddrinfo(ai_list) } -> std::same_as<void>;
        { t.gai_strerror(errcode) } -> std::same_as<char const *>;
    };


    requires requires(int domain, int type, int protocol, int sockfd,
                      int backlog)
    {
        { t.socket(domain, type, protocol) } -> std::same_as<int>;
        { t.close(sockfd) } -> std::same_as<int>;
        { t.listen(sockfd, backlog) } -> std::same_as<int>;
    };

    requires requires(int sockfd, typename T::SockAddr const * addr,
                      typename T::SockLen addrlen,
                      typename T::SockAddr* recv_addr,
                      typename T::SockLen* recv_addrlen)
    {
        { t.bind(sockfd, addr, addrlen) } -> std::same_as<int>;
        { t.connect(sockfd, addr, addrlen) } -> std::same_as<int>;
        { t.accept(sockfd, recv_addr, recv_addrlen) } -> std::same_as<int>;
    };

    requires requires(int sockfd, int level, int optname, void const * optval,
                      typename T::SockLen optlen, int cmd, int arg)
    {
        { t.setsockopt(sockfd, level, optname, optval, optlen) }
        -> std::same_as<int>;
        { t.fcntl(sockfd, cmd, arg) } -> std::same_as<int>;
    };

};

}  // namespace alewa::net
