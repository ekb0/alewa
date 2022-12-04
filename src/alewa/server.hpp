#pragma once

#include <string>
#include <vector>
#include <unordered_map>

#include "net/socket.hpp"
#include "io/ioapi.hpp"

namespace alewa {

extern int const AI_FLAGS;
extern int const AI_FAMILY;
extern int const AI_SOCKTYPE;
extern int const SOCK_LEVEL;
extern int const SOCK_OPTNAME;
extern int const F_CMD;
extern int const F_ARG;
extern short const INPUT_EVENTS;

template <net::PosixNetworkApi N, io::IoApi I>
class Server
{
private:
    using PollFd = typename I::PollFd;
    N const & netapi;
    I const & ioapi;

public:
    Server(N const & netapi, I const & ioapi) : netapi(netapi), ioapi(ioapi) {}
    void start(std::string const & port, int backlog);

private:
    auto create_listener(std::string const & port) -> net::Socket<N>;
    auto poll(std::vector<PollFd>& pollfds, int timeout) -> int;

    class Registry
    {
    private:
        std::unordered_map<int, net::Socket<N>> clients;
        std::vector<PollFd> pollfds;

    public:
        auto fds() noexcept -> std::vector<PollFd>& { return pollfds; }
        void add(net::Socket<N>&& client);
    };
};

template <net::PosixNetworkApi N, io::IoApi I>
void Server<N, I>::start(std::string const & port, int backlog)
{
    Registry registry;
    net::Socket<N> listener = create_listener(port);
    registry.fds().push_back({listener.fd(), INPUT_EVENTS, 0});
    listener.listen(backlog);

    int nready;
    for (;;) {
        nready = poll(registry.fds(), 0);  // TODO: retry on exception
        if (nready == 0) { continue; }

        if (registry.fds()[0].revents & INPUT_EVENTS) {
            net::SockInfo<N> client_info;
            net::Socket<N> client = listener.accept(client_info);
            registry.add(std::move(client));
        }
    }
}

template <net::PosixNetworkApi N, io::IoApi I>
auto Server<N, I>::create_listener(std::string const & port) -> net::Socket<N>
{
    typename N::AddrInfo hints{};
    hints.ai_flags = AI_FLAGS;
    hints.ai_family = AI_FAMILY;
    hints.ai_socktype = AI_SOCKTYPE;

    net::AddrInfoList<N> spec{netapi, nullptr, port.c_str(), &hints};
    net::Socket<N> socket{netapi, spec};
    socket.set_socket_option(SOCK_LEVEL, SOCK_OPTNAME, 1);
    socket.set_file_option(F_CMD, F_ARG);
    socket.bind(*spec.current());
    return socket;
}

template <net::PosixNetworkApi N, io::IoApi I>
auto Server<N, I>::poll(std::vector<PollFd>& pollfds, int timeout) -> int
{
    int const ret = ioapi.poll(&pollfds[0], pollfds.size(), timeout);
    if (ret == I::ERROR) {
        throw std::runtime_error{"poll failed: " + ioapi.error()};
    }
    return ret;
}

template <net::PosixNetworkApi N, io::IoApi I>
void Server<N, I>::Registry::add(net::Socket<N>&& client)
{
    assert(clients.find(client.fd()) == clients.end());
    pollfds.push_back({client.fd(), INPUT_EVENTS, 0});
    clients.insert(std::pair{client.fd(), std::move(client)});
}

}  // namespace alewa
