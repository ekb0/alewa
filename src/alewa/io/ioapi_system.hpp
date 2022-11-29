#pragma once

#include <poll.h>

#include "utils.hpp"

namespace alewa::io {

struct SystemIoApi
{
    using PollFd = ::pollfd;
    using Nfds_t = ::nfds_t;

    ALW_DELEGATE(poll, ::poll);
};

}  // namespace alewa::io