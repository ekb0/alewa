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
    using AddrInfo = typename T::AddrInfo;
    std::unique_ptr<AddrInfo, typename T::AIDeleter> p_ai;

public:
    AddrInfoList(T const & api, char const * p_node, char const * p_service,
                 AddrInfo const * p_hints);

    auto first() const noexcept -> AddrInfo const * { return p_ai.get(); }
};

template <AddrInfoProvider T>
AddrInfoList<T>::AddrInfoList(T const & api, char const * p_node,
                              char const * p_service, AddrInfo const * p_hints)
        : p_ai(nullptr, T::freeaddrinfo)
{
    AddrInfo * l = nullptr;
    int ret = api.getaddrinfo(p_node, p_service, p_hints, &l);
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
    using AddrInfo = typename T::AddrInfo;
    using SockAddr = typename T::SockAddr;
    using SockCloser = typename T::SockCloser;

    static int const NULL_FD = T::ERROR;

    int sockfd;
    SockCloser release;
    std::unique_ptr<AddrInfo const> ai;

    Socket(int sockfd, SockCloser const & closer, AddrInfo const & ai);

public:
    Socket(T const & api, AddrInfoList<U> const & ais);
    ~Socket();

    Socket(Socket&) = delete;
    Socket& operator=(Socket&) = delete;

    Socket(Socket&& other) noexcept;
    Socket& operator=(Socket&& other) noexcept;

    [[nodiscard]] auto fd() const noexcept -> int { return sockfd; }
    [[nodiscard]] auto info() const noexcept -> AddrInfo const & { return *ai; }

    void bind(T const & api, AddrInfo const & target);
    void connect(T const & api, AddrInfo const & target);

    void bind(T const & api) { bind(api, *ai); }
    void connect(T const & api) { connect(api, *ai); }

    void listen(T const & api, int backlog);
    Socket accept(T const & api);
};

template <SocketProvider T, AddrInfoProvider U>
Socket<T, U>::Socket(int sockfd, SockCloser const & closer, AddrInfo const & ai)
        : sockfd(sockfd), release(closer),
          ai(std::make_unique<AddrInfo const>(ai))
{}


template <SocketProvider T, AddrInfoProvider U>
Socket<T, U>::Socket(T const & api, AddrInfoList<U> const & ais)
        : release(T::close)
{
    int fd = T::ERROR;
    std::unique_ptr<AddrInfo> p_ai = nullptr;

    AddrInfo const * it;
    for (it = ais.first(); it != nullptr; it = it->ai_next) {
        fd = api.socket(it->ai_family, it->ai_socktype, it->ai_protocol);
        if (fd != T::ERROR) {
            p_ai = std::make_unique<AddrInfo>(*it);
            p_ai->ai_addr = nullptr;
            p_ai->ai_next = nullptr;
            break;
        }
    }

    if (fd == T::ERROR) {
        std::ostringstream err;
        err << "socket: " << api.error() << std::endl;
        throw std::runtime_error(err.str());
    }
    sockfd = fd;
    ai = std::move(p_ai);
}

template <SocketProvider T, AddrInfoProvider U>
Socket<T, U>::~Socket()
{
    if (sockfd != NULL_FD) {
        if (ai->ai_addr) { delete ai->ai_addr; }
        release(sockfd); /* TODO: stderr if this fails */
    }
}

template <SocketProvider T, AddrInfoProvider U>
Socket<T, U>::Socket(Socket&& other) noexcept
        : sockfd(other.sockfd), release(other.release), ai(std::move(other.ai))
{
    assert(ai != nullptr);
    other.sockfd = NULL_FD;
}

template <SocketProvider T, AddrInfoProvider U>
auto Socket<T, U>::operator=(Socket&& other) noexcept -> Socket<T, U>&
{
    assert(other.ai != nullptr);
    assert(&release == &other.release);
    std::swap(sockfd, other.sockfd);
    ai = std::move(other.ai);
    return *this;
}

template <SocketProvider T, AddrInfoProvider U>
void Socket<T, U>::bind(T const & api, AddrInfo const & target)
{
    int ret = api.bind(sockfd, target.ai_addr, target.ai_addrlen);
    if (ret == T::ERROR) {
        std::ostringstream err;
        err << "bind: " << api.error() << std::endl;
        throw std::runtime_error(err.str());
    }
}

template <SocketProvider T, AddrInfoProvider U>
void Socket<T, U>::connect(T const & api, AddrInfo const & target)
{
    int ret = api.connect(sockfd, target.ai_addr, target.ai_addrlen);
    if (ret == T::ERROR) {
        std::ostringstream err;
        err << "connect: " << api.error() << std::endl;
        throw std::runtime_error(err.str());
    }
}

template <SocketProvider T, AddrInfoProvider U>
void Socket<T, U>::listen(T const & api, int backlog)
{
    int ret = api.listen(sockfd, backlog);
    if (ret == T::ERROR) {
        std::ostringstream err;
        err << "listen: " << api.error() << std::endl;
        throw std::runtime_error(err.str());
    }
}

template <SocketProvider T, AddrInfoProvider U>
auto Socket<T, U>::accept(T const & api) -> Socket<T, U>
{
    AddrInfo tmp{};
    tmp.ai_addr = new SockAddr{};  /* lol, "modern" c++ */
    int fd = api.accept(sockfd, tmp.ai_addr, &tmp.ai_addrlen);
    if (fd == T::ERROR) {
        delete tmp.ai_addr;
        throw fd;
    }
    return Socket{fd, release, tmp};
}

}  // namespace alewa::net
