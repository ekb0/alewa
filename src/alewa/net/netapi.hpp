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
                      typename T::AddrInfo* ai_list)
    {
        { t.getaddrinfo(node, service, hints, p_ai_list) }
                -> std::same_as<int>;
        { t.freeaddrinfo(ai_list) } -> std::same_as<void>;
    };

    { t.gai_strerror(int{}) } -> std::same_as<char const *>;

    { t.socket(int{}, int{}, int{}) } -> std::same_as<int>;
    { t.close(int{}) } -> std::same_as<int>;
    { t.listen(int{}, int{}) } -> std::same_as<int>;

    requires requires(typename T::SockAddr const * addr,
                      typename T::SockLen addrlen,
                      typename T::SockAddr* recv_addr,
                      typename T::SockLen* recv_addrlen,
                      typename T::SockLen optlen,
                      void const * optval)
    {
        { t.bind(int{}, addr, addrlen) } -> std::same_as<int>;
        { t.connect(int{}, addr, addrlen) } -> std::same_as<int>;
        { t.accept(int{}, recv_addr, recv_addrlen) } -> std::same_as<int>;
        { t.setsockopt(int{}, int{}, int{}, optval, optlen) }
                -> std::same_as<int>;
    };

    { t.fcntl(int{}, int{}, int{}) } -> std::same_as<int>;
};

}  // namespace alewa::net
