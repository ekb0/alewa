#pragma once

#include <poll.h>

#include "sys/err_desc.hpp"

namespace alewa::io {

struct SystemIoApi : public sys::SystemErrorDescription
{
    using PollFd = ::pollfd;
    using Nfds = ::nfds_t;

    [[nodiscard]]
    auto poll(PollFd* fds, Nfds nfds, int timeout) const -> int
    {
        return ::poll(fds, nfds, timeout);
    }
};

}  // namespace alewa::io