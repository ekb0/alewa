#pragma once

#include <string>

namespace alewa::net::test {

struct MockNetworkApi
{
    struct SockAddr {
        unsigned short sa_family;
        char sa_data[14];
    };

    struct AddrInfo
    {
        int ai_family;
        int ai_socktype;
        int ai_protocol;
        unsigned short ai_addrlen;

        SockAddr* ai_addr;
        AddrInfo *ai_next = nullptr;
    };

    using AiDeleter = void(*)(AddrInfo*);
    using SockLen = unsigned short;

    static constexpr char const * const err = "Error";
    static int const ERROR = -1;
    static int const SUCCESS = 0;
    static int const ERRORNO = -5;

    static bool* p_is_freed;
    static bool* p_is_closed;

    mutable AddrInfo ai{};
    int ret_code = SUCCESS;
    int errorno = ERRORNO;

    static
    void set_is_freed(bool* val) { p_is_freed = val; }

    static
    void set_is_closed(bool* val) { p_is_closed = val; };

    [[nodiscard]]
    auto error() const -> std::string;

    [[nodiscard]]
    auto gai_strerror(int) const -> char const * { return err; }

    auto getaddrinfo(char const*, char const*, AddrInfo const*,
                    AddrInfo** p_ai_list) const -> int;

    static
    void freeaddrinfo(AddrInfo*);

    [[nodiscard]]
    auto socket(int, int, int) const -> int { return ret_code; }

    auto close(int) const -> int;

    auto bind(int, SockAddr const*, SockLen) const { return ret_code; }

    auto connect(int, SockAddr const*, SockLen) const { return ret_code; }

    auto listen(int, int) const { return ret_code; }

    auto accept(int, SockAddr* p_addr, SockLen* p_addrlen) const -> int;

    auto setsockopt(int, int, int, void const *, SockLen)
            const { return ret_code; }

    auto fcntl(int, int, int) const { return ret_code; }
};

}  // namespace alewa::net::test
