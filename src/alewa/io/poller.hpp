#pragma once

#include <cassert>
#include <vector>
#include <stdexcept>
#include "ioapi.hpp"

namespace alewa::io {

template <IoApi T>
class Poller
{
private:
    using PollFd = typename T::PollFd;
    T const & api;
    std::vector<PollFd> pfds;

public:
    explicit
    Poller(T const & api) : api(api){}

    auto poll(int timeout) -> int
    {
        int const ret = api.poll(&pfds[0], pfds.size(), timeout);
        if (ret == T::ERROR) {
            throw std::runtime_error{"poll failed: " + api.error()};
        }
        return ret;
    }

    auto fds() noexcept -> std::vector<PollFd>& { return pfds; }
};

}  // namespace alewa::io
