#pragma once

#include <concepts>

namespace alewa::net {

template <typename T>
concept ProviderBase = requires(T t)
{
    /* types */
    typename T::addrinfo;

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
    typename T::addrinfo_deleter;

    /* methods */
    requires requires(char const * node, char const * service,
                      typename T::addrinfo const * ref_hints,
                      typename T::addrinfo** ref_ai_list)
    {
        { t.getaddrinfo(node, service, ref_hints, ref_ai_list) }
        -> std::same_as<int>;
    };

    requires requires(typename T::addrinfo* ai_list)
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
    typename T::Closer;
    typename T::sockaddr;
    typename T::socklen_t;

    /* methods */
    { t.socket(int{}, int{}, int{}) } -> std::same_as<int>;

    { t.close(int{}) } -> std::same_as<int>;

    requires requires(int sockfd, typename T::sockaddr const * ref_addr,
                      typename T::socklen_t addrlen)
    {
        { t.bind(sockfd, ref_addr, addrlen) } -> std::same_as<int>;
        { t.connect(sockfd, ref_addr, addrlen) } -> std::same_as<int>;
    };
};

}  // namespace alewa::net
