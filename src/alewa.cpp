#include "net/sysnet_provider.hpp"

#include "server.hpp"

int main(/*int argc, char* argv[]*/)
{
    using namespace alewa;
    using namespace alewa::net;

    SystemNetworkProvider api;
    Server server{api};

    server.start("8080", 10);

    return 0;
}
