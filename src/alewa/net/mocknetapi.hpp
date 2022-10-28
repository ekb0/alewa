#include <cstddef>

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
    size_t ai_addrlen;
    char* ai_canonname;

    struct sockaddr_t* ai_addr;
    struct addrinfo *ai_next;
};

struct MockProviderBase
{
    using addrinfo = addrinfo_t;

    static constexpr char const * const err = "Error";

    static int const ERROR = -1;
    static int const SUCCESS = 0;

    [[nodiscard]] char const* neterror() const { return err; }
};

struct MockAddrInfoProvider : public MockProviderBase
{
    addrinfo ai{};
    addrinfo* ai_list = &ai;

    int getaddrinfo(char const*, char const*, addrinfo const*,
                    addrinfo** ref_ai) const
    {
        *ref_ai = ai_list;
        return SUCCESS;
    }

    static void freeaddrinfo(addrinfo*) {};
    [[nodiscard]] char const* gai_strerror(int) const { return err; }

    using addrinfo_deleter = decltype(&MockAddrInfoProvider::freeaddrinfo);
};

} // namespace alewa::net::test