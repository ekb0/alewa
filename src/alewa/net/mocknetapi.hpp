#pragma once

namespace alewa::net::test {

struct sockaddr_t
{
    unsigned short sa_family;
    char sa_data[14];
};

struct addrinfo_t
{
    int ai_flags;
    int ai_family;
    int ai_socktype;
    int ai_protocol;
    unsigned short ai_addrlen;
    char* ai_canonname;

    struct sockaddr_t* ai_addr;
    struct addrinfo_t *ai_next = nullptr;
};

struct MockProviderBase
{

    using addrinfo = addrinfo_t;

    static constexpr char const * const err = "Error";

    static int const ERROR = -1;
    static int const SUCCESS = 0;

    int ret_code = SUCCESS;

    [[nodiscard]] auto neterror() const -> char const *  { return err; }
};

struct MockAddrInfoProvider : public MockProviderBase
{
    using addrinfo_deleter [[maybe_unused]] = void(*)(addrinfo*);

    addrinfo ai{};
    addrinfo* ai_list = &ai;

    static bool* p_is_freed;
    static void set_is_freed(bool* val) { p_is_freed = val; }

    [[nodiscard]] char const* gai_strerror(int) const { return err; }

    int getaddrinfo(char const*, char const*, addrinfo const*,
                    addrinfo** ref_ai) const
    {
        *ref_ai = ai_list;
        return ret_code;
    }

    static void freeaddrinfo(addrinfo*)
    {
        if (p_is_freed != nullptr) {
            *p_is_freed = true;
        }
    }
};

inline bool* MockAddrInfoProvider::p_is_freed = nullptr;

struct MockSocketProvider : public MockProviderBase
{
    using sockaddr = sockaddr_t;
    using socklen_t = unsigned short;

    bool* p_is_closed = nullptr;
    void set_is_closed(bool* val) { p_is_closed = val; };

    [[nodiscard]] int socket (int, int, int) const { return ret_code; }
    int close(int) const
    {
        if (p_is_closed != nullptr) {
            *p_is_closed = true;
        }
        return ret_code;
    }

    int bind(int, sockaddr const*, socklen_t) const { return ret_code; }
    int connect(int, sockaddr const*, socklen_t) const { return ret_code; }
};

} // namespace alewa::net::test
