#include "net/network.hpp"
#include "net/sysnet_provider.hpp"

int main(/*int argc, char* argv[]*/)
{
    using namespace alewa::net;

    SystemNetworkProvider::AddrInfo hints{};
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    SystemNetworkProvider api;
    AddrInfoList<SystemNetworkProvider> spec{api, nullptr, "3490", &hints};
    Socket<SystemNetworkProvider> socket{api, spec};
    socket.bind(*spec.cur());

    return 0;
}
