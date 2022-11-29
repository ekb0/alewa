#pragma once

#include <string>

#include "net/socket.hpp"

namespace alewa {

namespace detail {

template <net::PosixNetworkApi T>
auto hints() -> typename T::AddrInfo;

}  // namespace alewa::detail

template <net::PosixNetworkApi T>
class Server
{
private:
    T const & api;

    auto new_bound_socket(std::string const & port) -> net::Socket<T>;

public:
    Server(T const & api) : api(api) {}

    void start(std::string const & port, int backlog);
};

template <net::PosixNetworkApi T>
void Server<T>::start(std::string const & port, int backlog)
{
    net::Socket<T> socket = new_bound_socket(port);
    socket.listen(backlog);
    net::SockInfo<T> info;
    net::Socket<T> accepted = socket.accept(info);
}

template <net::PosixNetworkApi T>
auto Server<T>::new_bound_socket(std::string const & port) -> net::Socket<T>
{
    typename T::AddrInfo hints = detail::hints<T>();
    net::AddrInfoList<T> spec{api, nullptr, port.c_str(), &hints};
    net::Socket<T> socket{api, spec};
    socket.bind(*spec.current());
    return socket;
}

namespace detail {
#include <netdb.h>

template <net::PosixNetworkApi T>
auto hints() -> typename T::AddrInfo
{
    typename T::AddrInfo hints{};
    hints.ai_family = AF_UNSPEC;
    hints.ai_flags = AI_PASSIVE;
    hints.ai_socktype = SOCK_STREAM;
    return hints;
}
}  // namespace alewa::detail

}  // namespace alewa
