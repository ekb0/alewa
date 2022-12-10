#pragma once

#include <string>
#include <vector>
#include <unordered_map>

#include "io/socket.hpp"
#include "io/ioapi.hpp"

namespace alewa {

namespace detail {
/* Expose the various macros used by the system network API without polluting
 * the main namespace with functions that mutate global state. */
#include <netdb.h>
#include <fcntl.h>
#include <poll.h>

static int const TCP_STREAM = SOCK_STREAM;
}  // namespace alewa::detail

template <io::IoApi T>
class Server
{
private:
    using PollFd = typename T::PollFd;
    T const & ioapi;

public:
    Server(T const & ioapi) : ioapi(ioapi) {}
    void start(std::string const & port, int backlog);

private:
    auto create_listener(std::string const & port) -> io::Socket<T>;
    auto poll(std::vector<PollFd>& pollfds, int timeout) -> int;

    class Registry
    {
    private:
        std::unordered_map<int, io::Socket<T>> clients;
        std::vector<PollFd> pollfds;

    public:
        auto fds() noexcept -> std::vector<PollFd>& { return pollfds; }
        void add(io::Socket<T>&& client);
    };
};

template <io::IoApi T>
void Server<T>::start(std::string const & port, int backlog)
{
    Registry registry;
    io::Socket<T> listener = create_listener(port);
    registry.fds().push_back({listener.fd(), POLLIN, 0});
    listener.listen(backlog);

    int nready;
    for (;;) {
        nready = poll(registry.fds(), 0);  // TODO: retry on exception
        if (nready == 0) { continue; }

        if (registry.fds()[0].revents & POLLIN) {
            io::SockInfo<T> client_info;
            io::Socket<T> client = listener.accept(client_info);
            registry.add(std::move(client));
        }
    }
}

template <io::IoApi T>
auto Server<T>::create_listener(std::string const & port) -> io::Socket<T>
{
    typename T::AddrInfo hints{};
    hints.ai_flags = AI_PASSIVE;
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = detail::TCP_STREAM;

    io::AddrInfoList<T> spec{ioapi, nullptr, port.c_str(), &hints};
    io::Socket<T> socket{ioapi, spec};
    socket.set_socket_option(SOL_SOCKET, SO_REUSEADDR, 1);
    socket.set_file_option(F_SETFL, O_NONBLOCK);
    socket.bind(*spec.current());
    return socket;
}

template <io::IoApi T>
auto Server<T>::poll(std::vector<PollFd>& pollfds, int timeout) -> int
{
    int const ret = ioapi.poll(&pollfds[0], pollfds.size(), timeout);
    if (ret == T::ERROR) {
        throw std::runtime_error{"poll failed: " + ioapi.error()};
    }
    return ret;
}

template <io::IoApi T>
void Server<T>::Registry::add(io::Socket<T>&& client)
{
    if (clients.find(client.fd()) == clients.end()) { return; }
    pollfds.push_back({client.fd(), POLLIN, 0});
    clients.insert(std::pair{client.fd(), std::move(client)});
}

}  // namespace alewa
