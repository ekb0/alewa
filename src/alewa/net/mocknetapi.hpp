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

    [[nodiscard]] char const* neterror() const { return err; }
};

struct MockAddrInfoProvider : public MockProviderBase
{
    addrinfo ai{};
    addrinfo* ai_list = &ai;

    static unsigned nfreed;

    int getaddrinfo(char const*, char const*, addrinfo const*,
                    addrinfo** ref_ai) const;

    static void freeaddrinfo(addrinfo*) { ++nfreed; };
    [[nodiscard]] char const* gai_strerror(int) const { return err; }

    using addrinfo_deleter [[maybe_unused]] =
            decltype(&MockAddrInfoProvider::freeaddrinfo);
};

struct MockSocketProvider : public MockProviderBase
{
    using sockaddr = sockaddr_t;
    using socklen_t = unsigned short;

    static unsigned nclosed;

    [[nodiscard]] int socket (int, int, int) const { return ret_code; }
    int close(int) const;

    int bind(int, sockaddr const*, socklen_t) const { return ret_code; }
    int connect(int, sockaddr const*, socklen_t) const { return ret_code; }
};

} // namespace alewa::net::test
