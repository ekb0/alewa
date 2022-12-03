#pragma once

#include <string>

#include "net/socket.hpp"
#include "io/poller.hpp"

namespace alewa {

extern int const AI_FLAGS;
extern int const AI_FAMILY;
extern int const AI_SOCKTYPE;

extern int const SOCK_LEVEL;
extern int const SOCK_OPTNAME;

extern int const F_CMD;
extern int const F_ARG;

extern short const INPUT_EVENTS;

template <net::PosixNetworkApi T, io::IoApi U>
class Server
{
private:
    T const & netapi;
    U const & ioapi;

public:
    Server(T const & netapi, U const & ioapi) : netapi(netapi), ioapi(ioapi) {}
    void start(std::string const & port, int backlog);

private:
    auto create_bound_socket(std::string const & port) -> net::Socket<T>;
    auto create_poller(int listen_fd) -> io::Poller<U>;
};

template <net::PosixNetworkApi T, io::IoApi U>
void Server<T, U>::start(std::string const & port, int backlog)
{
    net::Socket<T> socket = create_bound_socket(port);
    io::Poller<U> poller = create_poller(socket.fd());
    socket.listen(backlog);

    int nready;
    for (;;) {
        nready = poller.poll(0);
        if (nready > 0) {
            net::SockInfo<T> client_info;
            net::Socket<T> accepted = socket.accept(client_info);
        }
    }
}

template <net::PosixNetworkApi T, io::IoApi U>
auto Server<T, U>::create_bound_socket(std::string const & port) -> net::Socket<T>
{
    typename T::AddrInfo hints{};
    hints.ai_flags = AI_FLAGS;
    hints.ai_family = AI_FAMILY;
    hints.ai_socktype = AI_SOCKTYPE;

    net::AddrInfoList<T> spec{netapi, nullptr, port.c_str(), &hints};
    net::Socket<T> socket{netapi, spec};
    socket.set_option(SOCK_LEVEL, SOCK_OPTNAME, 1);
    socket.fcntl(F_CMD, F_ARG);
    socket.bind(*spec.current());
    return socket;
}

template <net::PosixNetworkApi T, io::IoApi U>
auto Server<T, U>::create_poller(int listen_fd) -> io::Poller<U>
{
    io::Poller<U> poller{ioapi};
    poller.fds().push_back({listen_fd, INPUT_EVENTS, 0});
    return poller;
}

}  // namespace alewa
