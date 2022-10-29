#pragma once

#include <string>
#include <memory>
#include <sstream>

#include "netapi.hpp"

namespace alewa::net {

template <AddrInfoProvider T>
class AddrInfoList
{
private:
    std::unique_ptr<typename T::addrinfo, typename T::addrinfo_deleter> p_ai;

public:
    AddrInfoList(T const & api, char const * node, char const * service,
                 typename T::addrinfo const & hints);

    typename T::addrinfo const * first() const { return p_ai.get(); }
};

template <AddrInfoProvider T>
AddrInfoList<T>::AddrInfoList(T const & api, char const * node,
                              char const * service,
                              typename T::addrinfo const & hints)
        : p_ai(nullptr, api.freeaddrinfo)
{
    typename T::addrinfo * l = nullptr;
    int ret = api.getaddrinfo(node, service, &hints, &l);
    if (ret != T::SUCCESS) {
        std::ostringstream err;
        err << "getaddrinfo: " << api.gai_strerror(ret) << std::endl;
        throw std::runtime_error(err.str());
    }
    p_ai.reset(l);
}

template <SocketProvider T, AddrInfoProvider U = T>
class Socket
{
private:
    T const & api;
    int sockfd;
    typename T::addrinfo ai;

public:
    Socket(T const & api, AddrInfoList<U> const & ais);
    ~Socket();

    Socket(Socket&) = delete;
    Socket& operator=(Socket&) = delete;

    Socket(Socket&&) noexcept = default;
    Socket& operator=(Socket&&) noexcept = default;

    [[nodiscard]] int fd() const { return sockfd; }
    [[nodiscard]] auto info() const -> typename T::addrinfo const &
    { return ai; }

    void bind();
    void connect();
};

template <SocketProvider T, AddrInfoProvider U>
Socket<T, U>::Socket(T const & api, AddrInfoList<U> const & ais) : api(api)
{
    int ret = T::ERROR;

    typename T::addrinfo const * it;
    for (it = ais.first(); it != nullptr; it = it->ai_next) {
        ret = api.socket(it->ai_family, it->ai_socktype, it->ai_protocol);
        if (ret != T::ERROR) { break; }
    }

    if (ret == T::ERROR) {
        std::ostringstream err;
        err << "socket: " << api.neterror() << std::endl;
        throw std::runtime_error(err.str());
    }
    sockfd = ret;
    ai = *it;
}

template <SocketProvider T, AddrInfoProvider U>
Socket<T, U>::~Socket()
{
    api.close(sockfd); /* TODO: stderr if this fails */
}

template <SocketProvider T, AddrInfoProvider U>
void Socket<T, U>::bind()
{
    int ret = api.bind(sockfd, ai.ai_addr, ai.ai_addrlen);
    if (ret == T::ERROR) {
        std::ostringstream err;
        err << "bind: " << api.neterror() << std::endl;
        throw std::runtime_error(err.str());
    }
}

template <SocketProvider T, AddrInfoProvider U>
void Socket<T, U>::connect()
{
    int ret = api.connect(sockfd, ai.ai_addr, ai.ai_addrlen);
    if (ret == T::ERROR) {
        std::ostringstream err;
        err << "connect: " << api.neterror() << std::endl;
        throw std::runtime_error(err.str());
    }
}

}  // namespace alewa::net
