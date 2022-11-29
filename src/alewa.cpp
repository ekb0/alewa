#include "net/netapi_system.hpp"

#include "server.hpp"

int main(/*int argc, char* argv[]*/)
{
    using namespace alewa;
    using namespace alewa::net;

    SystemNetworkApi api;
    Server<SystemNetworkApi> server{api};

    server.start("8080", 10);

    return 0;
}
