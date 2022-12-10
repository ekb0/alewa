#include "sockapi_mock.hpp"

namespace alewa::io::test {

bool* MockSocketApi::is_freed = nullptr;
bool* MockSocketApi::is_closed = nullptr;

auto MockSocketApi::error() const -> std::string
{
    return "Error: " + std::to_string(errorno);
}

auto MockSocketApi::getaddrinfo(char const*, char const*, AddrInfo const*,
                                AddrInfo** ai_list) const -> int
{
    *ai_list = &ai;
    return ret_code;
}

void MockSocketApi::freeaddrinfo(AddrInfo*)
{
    if (is_freed) { *is_freed = true; }
}

auto MockSocketApi::close(int) const -> int
{
    if (is_closed) { *is_closed = true; }
    return SUCCESS;
}

auto MockSocketApi::accept(int, SockAddr* addr, SockLen* addrlen) const
        -> int
{
    if (ret_code == ERROR) { return ret_code; }
    *addr = *ai.ai_addr;
    *addrlen = sizeof(SockLen);
    return ret_code;
}

}  // namespace alewa::io::test

