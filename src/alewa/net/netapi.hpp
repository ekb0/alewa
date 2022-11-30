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

    requires requires(char const * p_node, char const * p_service,
                      typename T::AddrInfo const * p_hints,
                      typename T::AddrInfo** p_ai_list,
                      typename T::AddrInfo* ai_list)
    {
        { t.getaddrinfo(p_node, p_service, p_hints, p_ai_list) }
                -> std::same_as<int>;
        { t.freeaddrinfo(ai_list) } -> std::same_as<void>;
    };

    { t.gai_strerror(int{}) } -> std::same_as<char const *>;

    { t.socket(int{}, int{}, int{}) } -> std::same_as<int>;
    { t.close(int{}) } -> std::same_as<int>;
    { t.listen(int{}, int{}) } -> std::same_as<int>;

    requires requires(typename T::SockAddr const * addr,
                      typename T::SockAddr* p_addr,
                      typename T::SockLen addrlen,
                      typename T::SockLen* p_addrlen,
                      typename T::SockLen optlen,
                      void const * p_optval)
    {
        { t.bind(int{}, addr, addrlen) } -> std::same_as<int>;
        { t.connect(int{}, addr, addrlen) } -> std::same_as<int>;
        { t.accept(int{}, p_addr, p_addrlen) } -> std::same_as<int>;
        { t.setsockopt(int{}, int{}, int{}, p_optval, optlen) }
                -> std::same_as<int>;
    };

    { t.fcntl(int{}, int{}, int{}) } -> std::same_as<int>;
};

}  // namespace alewa::net
