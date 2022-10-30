#pragma once

#include <concepts>

namespace alewa::net {

template <typename T>
concept ProviderBase = requires(T t)
{
    /* types */
    typename T::AddrInfo;

    /* constants */
    { T::ERROR } -> std::same_as<int const &>;
    { T::SUCCESS } -> std::same_as<int const &>;

    /* methods */
    { t.neterror() } -> std::same_as<char const *>;
};

template <typename T>
concept AddrInfoProvider= requires(T t)
{
    requires ProviderBase<T>;

    /* types */
    typename T::AIDeleter;

    /* methods */
    requires requires(char const * p_node, char const * p_service,
                      typename T::AddrInfo const * p_hints,
                      typename T::AddrInfo** p_ai_list)
    {
        { t.getaddrinfo(p_node, p_service, p_hints, p_ai_list) }
        -> std::same_as<int>;
    };

    requires requires(typename T::AddrInfo* ai_list)
    {
        { t.freeaddrinfo(ai_list) } -> std::same_as<void>;
    };

    { t.gai_strerror(int{}) } -> std::same_as<char const *>;
};

template <typename T>
concept SocketProvider = requires(T t)
{
    requires ProviderBase<T>;

    /* types */
    typename T::SockAddr;
    typename T::SockLen_t;
    typename T::SockCloser;

    /* methods */
    { t.socket(int{}, int{}, int{}) } -> std::same_as<int>;

    { t.close(int{}) } -> std::same_as<int>;

    requires requires(int sockfd, typename T::SockAddr const * addr,
                      typename T::SockLen_t addrlen)
    {
        { t.bind(sockfd, addr, addrlen) } -> std::same_as<int>;
        { t.connect(sockfd, addr, addrlen) } -> std::same_as<int>;
    };
};

}  // namespace alewa::net
