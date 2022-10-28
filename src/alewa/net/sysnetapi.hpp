#pragma once

#include <cerrno>
#include <cstring>
#include <netdb.h>

namespace alewa::net {

namespace details {

char const * neterror();

}  // namespace details

struct SysNetApi
{
    /* types */
    using addrinfo = ::addrinfo;
    using freeaddrinfo_t [[maybe_unused]] = decltype(&::freeaddrinfo);

    /* constants */
    static int const ERROR = -1;
    static int const SUCCESS = 0;

    /* methods */
    #define SYSNET_DELEGATE(func, delegate)                                    \
        decltype(&(delegate)) func = (delegate)

    SYSNET_DELEGATE(getaddrinfo, ::getaddrinfo);
    SYSNET_DELEGATE(freeaddrinfo, ::freeaddrinfo);
    SYSNET_DELEGATE(gai_strerror, ::gai_strerror);
    SYSNET_DELEGATE(socket, ::socket);
    SYSNET_DELEGATE(neterror, details::neterror);
};

}  // namespace alewa::net
