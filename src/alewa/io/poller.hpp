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
    std::vector<PollFd> pollfds;

public:
    explicit
    Poller(T const & api) : api(api){}

    auto poll(int timeout) -> int
    {
        int const ret = api.poll(&pollfds[0], pollfds.size(), timeout);
        if (ret == T::ERROR) {
            throw std::runtime_error{"poll failed: " + api.error()};
        }
        return ret;
    }

    auto fds() noexcept -> std::vector<PollFd>& { return pollfds; }
};

}  // namespace alewa::io
