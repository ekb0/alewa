#include "mocknetapi.hpp"

namespace alewa::net::test {

unsigned MockAddrInfoProvider::nfreed = 0;

int MockAddrInfoProvider::getaddrinfo(char const*, char const*, addrinfo const*,
                                      addrinfo** ref_ai) const
{
    *ref_ai = ai_list;
    return ret_code;
}

unsigned MockSocketProvider::nclosed = 0;

int MockSocketProvider::close(int) const
{
    ++nclosed;
    return ret_code;
}

} // namespace alewa::net::test
