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
    { t.error() } -> std::same_as<char const *>;
};

template <typename T>
concept AddrInfoProvider= requires(T const t)
{
    requires ProviderBase<T>;

    /* types */
    typename T::AIDeleter;

    /* methods */
    requires requires(char const * p_node, char const * p_service,
                      typename T::AddrInfo const * p_hints,
                      typename T::AddrInfo** p_ai_list,
                      typename T::AddrInfo* ai_list)
    {
        { t.getaddrinfo(p_node, p_service, p_hints, p_ai_list) }
        -> std::same_as<int>;
        { T::freeaddrinfo(ai_list) } -> std::same_as<void>;
    };

    { t.gai_strerror(int{}) } -> std::same_as<char const *>;
};

template <typename T>
concept SocketProvider = requires(T const t)
{
    requires ProviderBase<T>;

    /* types */
    typename T::SockAddr;
    typename T::SockLen_t;
    typename T::SockCloser;

    /* methods */
    { t.socket(int{}, int{}, int{}) } -> std::same_as<int>;
    { T::close(int{}) } -> std::same_as<int>;

    requires requires(typename T::SockAddr const * addr,
                      typename T::SockAddr* p_addr,
                      typename T::SockLen_t addrlen,
                      typename T::SockLen_t* p_addrlen)
    {
        { t.bind(int{}, addr, addrlen) } -> std::same_as<int>;
        { t.connect(int{}, addr, addrlen) } -> std::same_as<int>;
        { t.listen(int{}, int{}) } -> std::same_as<int>;
        { t.accept(int{}, p_addr, p_addrlen) } -> std::same_as<int>;
    };
};

}  // namespace alewa::net
