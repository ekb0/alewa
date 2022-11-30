#pragma once

#include <poll.h>

#include "utils.hpp"
#include "sys/err_desc.hpp"

namespace alewa::io {

struct SystemIoApi : public sys::SystemErrorDescription
{
    using PollFd = ::pollfd;
    using Nfds = ::nfds_t;

    ALW_DELEGATE(poll, ::poll);
};

}  // namespace alewa::io