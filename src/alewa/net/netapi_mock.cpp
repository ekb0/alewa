#include "netapi_mock.hpp"

namespace alewa::net::test {

bool* MockNetworkApi::p_is_freed = nullptr;
bool* MockNetworkApi::p_is_closed = nullptr;

auto MockNetworkApi::error() const -> std::string
{
    return "Error: " + std::to_string(errorno);
}

auto MockNetworkApi::getaddrinfo(char const*, char const*, AddrInfo const*,
                AddrInfo** p_ai_list) const -> int
{
    *p_ai_list = &ai;
    return ret_code;
}

void MockNetworkApi::freeaddrinfo(AddrInfo*)
{
    if (p_is_freed) { *p_is_freed = true; }
}

auto MockNetworkApi::close(int) const -> int
{
    if (p_is_closed) { *p_is_closed = true; }
    return SUCCESS;
}

auto MockNetworkApi::accept(int, SockAddr* p_addr, SockLen* p_addrlen) const
        -> int
{
    if (ret_code == ERROR) { return ret_code; }
    *p_addr = *ai.ai_addr;
    *p_addrlen = sizeof(SockLen);
    return ret_code;
}

}  // namespace alewa::net::test

