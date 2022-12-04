#pragma once

#include <concepts>
#include "sys/err_desc.hpp"

namespace alewa::io {

template <typename T>
concept IoApi = requires(T t)
{
    requires alewa::sys::ErrorDescription<T>;

    typename T::PollFd;
    typename T::Nfds;

    requires requires (typename T::PollFd* fds, typename T::Nfds nfds,
                       int timeout)
    {
        { t.poll(fds, nfds, timeout) } -> std::same_as<int>;
    };
};

}  // namespace alewa::io
