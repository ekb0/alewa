#pragma once

#include <cerrno>
#include <cstring>
#include <netdb.h>
#include <unistd.h>

namespace alewa::net {

struct SysNetApi
{
    /* types */
    using addrinfo = ::addrinfo;
    using addrinfo_deleter [[maybe_unused]] = decltype(&::freeaddrinfo);

    /* constants */
    static int const ERROR = -1;
    static int const SUCCESS = 0;

    /* methods */
    [[nodiscard]] char const * neterror() const;

    #define SYSNET_DELEGATE(func, delegate)                                    \
        decltype(&(delegate)) func = (delegate)

    SYSNET_DELEGATE(getaddrinfo, ::getaddrinfo);
    SYSNET_DELEGATE(freeaddrinfo, ::freeaddrinfo);
    SYSNET_DELEGATE(gai_strerror, ::gai_strerror);
    SYSNET_DELEGATE(socket, ::socket);
    SYSNET_DELEGATE(close, ::close);
};

}  // namespace alewa::net
