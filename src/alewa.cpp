#include "net/netapi_system.hpp"

#include "server.hpp"

int main(/*int argc, char* argv[]*/)
{
    using namespace alewa;
    using namespace alewa::net;

    std::string const PORT = "8080";
    int const BACKLOG = 10;

    SystemNetworkApi api;

    Server<SystemNetworkApi> server;
    server.start(api, PORT, BACKLOG);

    return 0;
}
