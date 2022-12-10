#include "io/ioapi_sys.hpp"
#include "server.hpp"

int main(/*int argc, char* argv[]*/)
{
    using namespace alewa;

    std::string const PORT = "8080";
    int const BACKLOG = 10;

    io::SysIoApi ioapi;

    Server<io::SysIoApi> server{ioapi};
    server.start(PORT, BACKLOG);

    return 0;
}
