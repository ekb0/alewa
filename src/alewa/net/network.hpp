#pragma once

#include <memory>
#include <cassert>

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
        : p_ai(nullptr, api.freeaddrinfo)
{
    AddrInfo* l = nullptr;
    int ret = api.getaddrinfo(p_node, p_service, p_hints, &l);
    if (ret != T::SUCCESS) { throw ret; }
    p_ai.reset(l);
}

template <SocketProvider T, AddrInfoProvider U = T>
class Socket
{
private:
    using AddrInfo = typename T::AddrInfo;
    using SockAddr = typename T::SockAddr;

    static int const NULL_FD = T::ERROR;

    T const & api;
    int sockfd;
    std::unique_ptr<AddrInfo const> ai;

    Socket(T const & api, int sockfd, AddrInfo const & ai);

public:
    Socket(T const & api, AddrInfoList<U> const & ais);
    ~Socket();

    Socket(Socket&) = delete;
    Socket& operator=(Socket&) = delete;

    Socket(Socket&& other) noexcept;
    Socket& operator=(Socket&& other) noexcept;

    [[nodiscard]] auto fd() const noexcept -> int { return sockfd; }
    [[nodiscard]] auto info() const noexcept -> AddrInfo const & { return *ai; }

    void bind(AddrInfo const & target);
    void connect(AddrInfo const & target);

    void bind() { bind(*ai); }
    void connect() { connect(*ai); }

    void listen(int backlog);
    auto accept() -> Socket;
};

template <SocketProvider T, AddrInfoProvider U>
Socket<T, U>::Socket(T const & api, int sockfd, AddrInfo const & ai)
        : api(api), sockfd(sockfd), ai(std::make_unique<AddrInfo const>(ai))
{ }


template <SocketProvider T, AddrInfoProvider U>
Socket<T, U>::Socket(T const & api, AddrInfoList<U> const & ais) : api(api)
{
    AddrInfo const * it = ais.first();
    while (it) {
        sockfd = api.socket(it->ai_family, it->ai_socktype, it->ai_protocol);
        if (sockfd == NULL_FD) {
            it = it->ai_next;
            continue;
        }
        std::unique_ptr<AddrInfo> p_ai = std::make_unique<AddrInfo>(*it);
        p_ai->ai_addr = nullptr;
        p_ai->ai_next = nullptr;

        ai = std::move(p_ai);
        return;
    }
    throw api.err_no();
}

template <SocketProvider T, AddrInfoProvider U>
Socket<T, U>::~Socket()
{
    if (sockfd == NULL_FD) {
        assert(!ai);
        return;
    }
    if (ai && ai->ai_addr) { delete ai->ai_addr; }
    api.close(sockfd); /* TODO: stderr if this fails */
}

template <SocketProvider T, AddrInfoProvider U>
Socket<T, U>::Socket(Socket&& other) noexcept
        : api(other.api), sockfd(other.sockfd), ai(std::move(other.ai))
{
    assert(&api == &other.api);
    other.sockfd = NULL_FD;
}

template <SocketProvider T, AddrInfoProvider U>
auto Socket<T, U>::operator=(Socket&& other) noexcept -> Socket<T, U>&
{
    std::swap(sockfd, other.sockfd);
    ai = std::move(other.ai);
    return *this;
}

template <SocketProvider T, AddrInfoProvider U>
void Socket<T, U>::bind(AddrInfo const & target)
{
    int ret = api.bind(sockfd, target.ai_addr, target.ai_addrlen);
    if (ret == T::ERROR) { throw api.err_no(); }
}

template <SocketProvider T, AddrInfoProvider U>
void Socket<T, U>::connect(AddrInfo const & target)
{
    int ret = api.connect(sockfd, target.ai_addr, target.ai_addrlen);
    if (ret == T::ERROR) { throw api.err_no(); }
}

template <SocketProvider T, AddrInfoProvider U>
void Socket<T, U>::listen(int backlog)
{
    int ret = api.listen(sockfd, backlog);
    if (ret == T::ERROR) { throw api.err_no(); }
}

template <SocketProvider T, AddrInfoProvider U>
auto Socket<T, U>::accept() -> Socket<T, U>
{
    AddrInfo tmp{};
    tmp.ai_addr = new SockAddr{};  /* "modern" c++ ;) */
    int fd = api.accept(sockfd, tmp.ai_addr, &tmp.ai_addrlen);
    if (fd == NULL_FD) {
        delete tmp.ai_addr;
        throw api.err_no();
    }
    return Socket{api, fd, tmp};
}

}  // namespace alewa::net
