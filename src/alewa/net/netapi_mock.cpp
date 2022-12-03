#include "netapi_mock.hpp"

namespace alewa::net::test {

bool* MockNetworkApi::is_freed = nullptr;
bool* MockNetworkApi::is_closed = nullptr;

auto MockNetworkApi::error() const -> std::string
{
    return "Error: " + std::to_string(errorno);
}

auto MockNetworkApi::getaddrinfo(char const*, char const*, AddrInfo const*,
                AddrInfo** ai_list) const -> int
{
    *ai_list = &ai;
    return ret_code;
}

void MockNetworkApi::freeaddrinfo(AddrInfo*)
{
    if (is_freed) { *is_freed = true; }
}

auto MockNetworkApi::close(int) const -> int
{
    if (is_closed) { *is_closed = true; }
    return SUCCESS;
}

auto MockNetworkApi::accept(int, SockAddr* addr, SockLen* addrlen) const
        -> int
{
    if (ret_code == ERROR) { return ret_code; }
    *addr = *ai.ai_addr;
    *addrlen = sizeof(SockLen);
    return ret_code;
}

}  // namespace alewa::net::test

