#pragma once

#include <concepts>

namespace alewa::net {

namespace _sysnet {
/*
 * Private namespace to keep the symbols in system network headers out of the
 * global namespace
 */
#include <netdb.h>
}

template <typename T, typename U, typename... Args>
concept SameAsRetType = requires(U u, Args... args)
{
    requires std::same_as<T, decltype(std::declval(u(args...)))>;
};

template <typename T, typename... Args>
concept NetApi = requires(T t, Args... args)
{
#define NETAPI_REQUIRE(func)                                                   \
    { t.func(args...) } -> SameAsRetType<decltype(_sysnet::func)>

    NETAPI_REQUIRE(freeaddrinfo);
    NETAPI_REQUIRE(gai_strerror);
};

class NetApiImpl
{
#define NETAPI_DELEGATE(func)                                                  \
    template <typename... Args>                                                \
    auto (func)(Args... args) { return (_sysnet::func)(args...); }             \
                                                                               \
    static_assert(true, "") /* require a semicolon after using macro */

public:
    NETAPI_DELEGATE(freeaddrinfo);
    NETAPI_DELEGATE(gai_strerror);
};

}  // namespace alewa::net
