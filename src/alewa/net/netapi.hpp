#pragma once

#include <concepts>

namespace alewa::net {

template <typename T>
concept NetApi = requires(T t)
{
    /* types */
    typename T::addrinfo;
    typename T::addrinfo_deleter;

    /* constants */
    { T::ERROR } -> std::same_as<int const &>;
    { T::SUCCESS } -> std::same_as<int const &>;

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

    { t.socket(int{}, int{}, int{}) } -> std::same_as<int>;

    { t.close(int{}) } -> std::same_as<int>;

    { t.neterror() } -> std::same_as<char const *>;

};

}  // namespace alewa::net
