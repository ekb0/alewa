#pragma once

#include <string>
#include <unordered_map>

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
    auto create_listener(std::string const & port) -> net::Socket<T>;

    class Registry
    {
    private:
        std::unordered_map<int, net::Socket<T>> clients;
        std::vector<typename U::PollFd>& pollfds;

    public:
        explicit
        Registry(std::vector<typename U::PollFd>& fds) : pollfds(fds) {}

        void add(net::Socket<T>&& client);
    };
};

template <net::PosixNetworkApi T, io::IoApi U>
void Server<T, U>::start(std::string const & port, int backlog)
{
    io::Poller<U> poller{ioapi};
    Registry registry{poller.fds()};

    net::Socket<T> listener = create_listener(port);
    poller.fds().push_back({listener.fd(), INPUT_EVENTS, 0});
    listener.listen(backlog);

    int nready;
    for (;;) {
        nready = poller.poll(0);  // TODO: retry on exception
        if (nready == 0) { continue; }

        if (poller.fds()[0].revents & INPUT_EVENTS) {
            net::SockInfo<T> client_info;
            net::Socket<T> client = listener.accept(client_info);
            registry.add(std::move(client));
        }
    }
}

template <net::PosixNetworkApi T, io::IoApi U>
auto Server<T, U>::create_listener(std::string const & port) -> net::Socket<T>
{
    typename T::AddrInfo hints{};
    hints.ai_flags = AI_FLAGS;
    hints.ai_family = AI_FAMILY;
    hints.ai_socktype = AI_SOCKTYPE;

    net::AddrInfoList<T> spec{netapi, nullptr, port.c_str(), &hints};
    net::Socket<T> socket{netapi, spec};
    socket.set_socket_option(SOCK_LEVEL, SOCK_OPTNAME, 1);
    socket.set_file_option(F_CMD, F_ARG);
    socket.bind(*spec.current());
    return socket;
}

template <net::PosixNetworkApi T, io::IoApi U>
void Server<T, U>::Registry::add(net::Socket<T>&& client)
{
    assert(clients.find(client.fd()) == clients.end());
    pollfds.push_back({client.fd(), INPUT_EVENTS, 0});
    clients.insert(std::pair{client.fd(), std::move(client)});
}

}  // namespace alewa
