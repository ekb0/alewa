#include "net/network.hpp"
#include "net/sysnetapi.hpp"

int main(/*int argc, char* argv[]*/)
{
    using namespace alewa::net;

    SysNetApi::AddrInfo hints{};
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    SysNetApi api;
    AddrInfoList<SysNetApi> spec{api, nullptr, "3490", &hints};
    Socket<SysNetApi> socket{api, spec};
    socket.bind(*spec.cur());

    return 0;
}
