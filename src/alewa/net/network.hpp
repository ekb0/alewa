#pragma once

#include <string>
#include <memory>
#include <sstream>

#include "netapi.hpp"

namespace alewa::net {

template <NetApi T>
class AddrInfoList
{
private:
    std::unique_ptr<typename T::addrinfo, typename T::addrinfo_deleter> p_ai;

public:
    AddrInfoList(T const & api, char const * node, char const * service,
                 typename T::addrinfo const & hints);

    typename T::addrinfo const * first() const { return p_ai.get(); }
};

template <NetApi T>
AddrInfoList<T>::AddrInfoList(T const & api, char const * node,
                              char const * service,
                              typename T::addrinfo const & hints)
        : p_ai(nullptr, api.freeaddrinfo)
{
    typename T::addrinfo * l = nullptr;
    int ret = api.getaddrinfo(node, service, &hints, &l);
    if (ret != T::SUCCESS) {
        std::ostringstream err;
        err << "getaddrinfo: " << api.gai_strerror(ret) << "\n";
        throw std::runtime_error(err.str());
    }
    p_ai.reset(l);
}

template <NetApi T>
class Socket
{
private:
    T const & api;
    int sockfd;

public:
    Socket(T const & api, AddrInfoList<T> const & ais);
    ~Socket();

    Socket(Socket&) = delete;
    Socket& operator=(Socket&) = delete;

    Socket(Socket&&) noexcept = default;
    Socket& operator=(Socket&&) noexcept = default;
};

template <NetApi T>
Socket<T>::Socket(T const & api, AddrInfoList<T> const & ais) : api(api)
{
    int ret = T::ERROR;

    typename T::addrinfo const * it;
    for (it = ais.first(); it != nullptr; it = it->ai_next) {
        ret = api.socket(it->ai_family, it->ai_socktype, it->ai_protocol);
        if (ret != T::ERROR) { break; }
    }

    if (ret == T::ERROR) {
        std::ostringstream err;
        err << "socket: " << api.neterror();
        throw std::runtime_error(err.str());
    }
    sockfd = ret;
}

template <NetApi T>
Socket<T>::~Socket()
{
    api.close(sockfd); /* TODO: stderr if this fails */
}

}  // namespace alewa::net
