#pragma once

#include <string>

#include "net/socket.hpp"

namespace alewa {

namespace detail {

template <net::PosixNetworkApi T>
net::Socket<T> new_bound_socket(T const & api, std::string const & port);

}  // namespace alewa::detail

template <net::PosixNetworkApi T>
class Server
{
public:
    void start(T const & api, std::string const & port, int backlog);
};

template <net::PosixNetworkApi T>
void Server<T>::start(T const & api, std::string const & port, int backlog)
{
    net::Socket<T> socket = detail::new_bound_socket(api, port);
    socket.listen(backlog);
    net::SockInfo<T> info;
    net::Socket<T> accepted = socket.accept(info);
}

namespace detail {

/*
 * only constants from these headers should be referenced. methods that modify
 * system singletons should be accessed from the network api instance
 */
#include <netdb.h>
#include <fcntl.h>

template <net::PosixNetworkApi T>
auto new_bound_socket(T const & api, std::string const & port)
        -> net::Socket<T>
{
    typename T::AddrInfo hints{};
    hints.ai_family = AF_UNSPEC;
    hints.ai_flags = AI_PASSIVE;
    hints.ai_socktype = SOCK_STREAM;

    net::AddrInfoList<T> spec{api, nullptr, port.c_str(), &hints};

    net::Socket<T> socket{api, spec};
    socket.set_option(SOL_SOCKET, SO_REUSEADDR, 1);
    socket.fcntl(F_SETFL, O_NONBLOCK);
    socket.bind(*spec.current());

    return socket;
}

}  // namespace alewa::detail

}  // namespace alewa
