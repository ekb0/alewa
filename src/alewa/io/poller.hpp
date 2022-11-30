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
    Poller(T const & api) : api(api){}

    auto poll(int timeout) -> int;
    void add(int fd, short events);
    void del(int idx);
};

template <IoApi T>
auto Poller<T>::poll(int timeout) -> int
{
    int const ret = api.poll(&pfds[0], pfds.size(), timeout);
    if (ret == T::ERROR) {
        throw std::runtime_error{"poll failed: " + api.error()};
    }
    return ret;
}

template <IoApi T>
void Poller<T>::add(int fd, short events)
{
    pfds.emplace_back({fd, events, 0});
}

template <IoApi T>
void Poller<T>::del(int idx)
{
    assert(0 <= idx && idx < pfds.size());
    pfds[idx] = std::move(pfds.back());
    pfds.pop_back();
}

}  // namespace alewa::io
