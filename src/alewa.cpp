#include "net/network.hpp"
#include "net/sysnetapi.hpp"

int main(/*int argc, char* argv[]*/)
{
    using namespace alewa::net;

    SysNetApi::addrinfo hints{};
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    SysNetApi api;
    AddrInfo<SysNetApi> ai{api, nullptr, "3490", hints};

    return 0;
}
