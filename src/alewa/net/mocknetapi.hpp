#pragma once

namespace alewa::net::test {

/*
 * WARNING:
 * The providers in this class are stack based. When used for tests, they should
 * never go out of scope before the instances they have been used to create.
 */

struct sockaddr
{
    unsigned short sa_family;
    char sa_data[14];
};

struct addrinfo
{
    int ai_flags;
    int ai_family;
    int ai_socktype;
    int ai_protocol;
    unsigned short ai_addrlen;
    char* ai_canonname;

    struct sockaddr* ai_addr;
    struct addrinfo *ai_next = nullptr;
};

struct MockProviderBase
{

    using AddrInfo = addrinfo;

    static int const ERROR = -1;
    static int const SUCCESS = 0;

    [[nodiscard]] auto error() const -> char const * { return err; }

    static constexpr char const * const err = "Error";
    int ret_code = SUCCESS;
};

struct MockAddrInfoProvider : public MockProviderBase
{
    using AIDeleter [[maybe_unused]] = void(*)(AddrInfo*);

    mutable AddrInfo ai{};

    static bool* p_is_freed;
    static void set_is_freed(bool* val) { p_is_freed = val; }

    [[nodiscard]] auto gai_strerror(int) const -> char const * { return err; }

    int getaddrinfo(char const*, char const*, AddrInfo const*,
                    AddrInfo** p_ai_list) const
    {
        *p_ai_list = &ai;
        return ret_code;
    }

    static void freeaddrinfo(AddrInfo*)
    {
        if (p_is_freed) { *p_is_freed = true; }
    }
};

inline bool* MockAddrInfoProvider::p_is_freed = nullptr;

struct MockSocketProvider : public MockProviderBase
{
    using SockAddr = sockaddr;
    using SockLen_t = unsigned short;
    using SockCloser [[maybe_unused]] = int(*)(int);

    static bool* p_is_closed;
    static void set_is_closed(bool* val) { p_is_closed = val; };

    [[nodiscard]] int socket (int, int, int) const { return ret_code; }

    static int close(int)
    {
        if (p_is_closed) { *p_is_closed = true; }
        return SUCCESS;
    }

    int bind(int, SockAddr const*, SockLen_t) const { return ret_code; }
    int connect(int, SockAddr const*, SockLen_t) const { return ret_code; }
};

inline bool* MockSocketProvider::p_is_closed = nullptr;

} // namespace alewa::net::test
