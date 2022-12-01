#include "net/netapi_system.hpp"
#include "io/ioapi_system.hpp"
#include "server.hpp"

int main(/*int argc, char* argv[]*/)
{
    using namespace alewa;

    std::string const PORT = "8080";
    int const BACKLOG = 10;

    net::SystemNetworkApi netapi;
    io::SystemIoApi ioapi;

    Server<net::SystemNetworkApi, io::SystemIoApi> server{netapi, ioapi};
    server.start(PORT, BACKLOG);

    return 0;
}
