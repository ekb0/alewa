#pragma once

#include <string>

namespace alewa::net::test {

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

    sockaddr* ai_addr;
    addrinfo *ai_next = nullptr;
};

struct MockNetworkApi
{
    using AddrInfo = addrinfo;
    using AiDeleter = void(*)(AddrInfo*);
    using SockAddr = sockaddr;
    using SockLen_t = unsigned short;

    mutable AddrInfo ai{};

    static int const ERROR;
    static int const SUCCESS;
    static int const ERRORNO;

    [[nodiscard]] auto error(int val) const -> std::string
    {
        return "Error: " + std::to_string(val);
    }

    [[nodiscard]] int err_no() const { return errorno; }

    static constexpr char const * const err = "Error";
    int ret_code = SUCCESS;
    int errorno = ERRORNO;


    static bool* p_is_freed;
    static void set_is_freed(bool* val) { p_is_freed = val; }

    [[nodiscard]] auto gai_error(int) const -> char const * { return err; }

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

    static bool* p_is_closed;
    static void set_is_closed(bool* val) { p_is_closed = val; };

    [[nodiscard]] int socket (int, int, int) const { return ret_code; }

    int close(int) const
    {
        if (p_is_closed) { *p_is_closed = true; }
        return SUCCESS;
    }

    int bind(int, SockAddr const*, SockLen_t) const { return ret_code; }
    int connect(int, SockAddr const*, SockLen_t) const { return ret_code; }
    int listen(int, int) const { return ret_code; }

    int accept(int, SockAddr* p_addr, SockLen_t* p_addrlen) const
    {
        if (ERROR == ret_code) { return ret_code; }
        *p_addr = *ai.ai_addr;
        *p_addrlen = sizeof(SockLen_t);
        return ret_code;
    }
};

inline int const MockNetworkApi::ERROR = -1;
inline int const MockNetworkApi::SUCCESS = 0;
inline int const MockNetworkApi::ERRORNO = -5;

inline bool* MockNetworkApi::p_is_freed = nullptr;
inline bool* MockNetworkApi::p_is_closed = nullptr;

} // namespace alewa::net::test
