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

    auto first() const -> typename T::addrinfo const *
    {
        return p_ai.get();
    }
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
    static int const NULL_FD = -1;

    int sockfd;
    T const & api;
    std::unique_ptr<typename T::addrinfo> ref_ai;

public:
    Socket(T const & api, AddrInfoList<U> const & ais);
    ~Socket();

    Socket(Socket&) = delete;
    Socket& operator=(Socket&) = delete;

    Socket(Socket&& other) noexcept;
    Socket& operator=(Socket&& other) noexcept;

    [[nodiscard]] auto fd() const -> int { return sockfd; }
    [[nodiscard]] auto info() const -> typename T::addrinfo const &
    {
        return *ref_ai;
    }

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
    ref_ai = std::make_unique<typename T::addrinfo>(*it);
    ref_ai->ai_next = nullptr;
}

template <SocketProvider T, AddrInfoProvider U>
Socket<T, U>::~Socket()
{
    if (sockfd != NULL_FD) { api.close(sockfd); } // TODO: stderr if this fails
}

template <SocketProvider T, AddrInfoProvider U>
Socket<T, U>::Socket(Socket&& other) noexcept
        : sockfd(other.sockfd), api(other.api), ref_ai(std::move(other.ref_ai))
{
    assert(ref_ai != nullptr);
    other.sockfd = NULL_FD;
}

template <SocketProvider T, AddrInfoProvider U>
auto Socket<T,U>::operator=(Socket&& other) noexcept -> Socket<T, U>&
{
    assert(other.ref_ai != nullptr);
    this->api = other.api;
    this->ref_ai = std::move(other.ref_ai);
    std::swap(this->sockfd, other.sockfd);
    return *this;
}

template <SocketProvider T, AddrInfoProvider U>
void Socket<T, U>::bind()
{
    int ret = api.bind(sockfd, ref_ai->ai_addr, ref_ai->ai_addrlen);
    if (ret == T::ERROR) {
        std::ostringstream err;
        err << "bind: " << api.neterror() << std::endl;
        throw std::runtime_error(err.str());
    }
}

template <SocketProvider T, AddrInfoProvider U>
void Socket<T, U>::connect()
{
    int ret = api.connect(sockfd, ref_ai->ai_addr, ref_ai->ai_addrlen);
    if (ret == T::ERROR) {
        std::ostringstream err;
        err << "connect: " << api.neterror() << std::endl;
        throw std::runtime_error(err.str());
    }
}

}  // namespace alewa::net
