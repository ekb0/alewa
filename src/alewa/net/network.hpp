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

    std::unique_ptr<AddrInfo, typename T::AIDeleter> head;
    AddrInfo const * iter;

public:
    AddrInfoList(T const & api, char const * p_node, char const * p_service,
                 AddrInfo const * p_hints)
            : head(nullptr, api.freeaddrinfo)
    {
        AddrInfo* l = nullptr;
        int ret = api.getaddrinfo(p_node, p_service, p_hints, &l);
        if (T::SUCCESS != ret) { throw ret; }
        head.reset(l);
        iter = l;
    }

    auto cur() const noexcept -> AddrInfo const *
    {
        return iter;
    }

    auto advance() noexcept -> AddrInfo const *
    {
        assert(iter);
        iter = iter->ai_next;
        return iter;
    }

    [[maybe_unused]] void reset() noexcept { iter = head.get(); }
};

template <SocketProvider T>
struct SockInfo
{
    typename T::SockAddr addr;
    typename T::SockLen_t addrlen;
};

template <SocketProvider T, AddrInfoProvider U = T>
class Socket
{
private:
    using AddrInfo = typename T::AddrInfo;

    static int const NULL_FD = T::ERROR;
    T const & api;
    int sockfd;

    Socket(T const & api, int sockfd) : api(api), sockfd(sockfd) {};

public:
    Socket(T const & api, AddrInfoList<U>& spec);
    ~Socket();

    Socket(Socket&) = delete;
    Socket& operator=(Socket&) = delete;

    Socket(Socket&& other) noexcept;
    Socket& operator=(Socket&& other) noexcept;

    void bind(AddrInfoList<U> const & target);
    void connect(AddrInfoList<U> const & target);

    void bind(AddrInfo const & target);
    void connect(AddrInfo const & target);

    void listen(int backlog);
    auto accept(SockInfo<T>& sockinfo) -> Socket;

    [[nodiscard]] auto fd() const noexcept -> int { return sockfd; }
};

template <SocketProvider T, AddrInfoProvider U>
Socket<T, U>::Socket(T const & api, AddrInfoList<U>& spec) : api(api)
{
    AddrInfo const * it = spec.cur();
    for (; it != nullptr; it = spec.advance()) {
        sockfd = api.socket(it->ai_family, it->ai_socktype, it->ai_protocol);
        if (NULL_FD != sockfd) { return; }
    }
    throw api.err_no();
}

template <SocketProvider T, AddrInfoProvider U>
Socket<T, U>::~Socket()
{
    if (NULL_FD == sockfd) { return; }
    api.close(sockfd); /* TODO: stderr if this fails */
}

template <SocketProvider T, AddrInfoProvider U>
Socket<T, U>::Socket(Socket&& other) noexcept
        : api(other.api), sockfd(other.sockfd)
{
    other.sockfd = NULL_FD;
}

template <SocketProvider T, AddrInfoProvider U>
auto Socket<T, U>::operator=(Socket&& other) noexcept -> Socket<T, U>&
{
    assert(&api == &other.api);
    std::swap(sockfd, other.sockfd);
    return *this;
}

template <SocketProvider T, AddrInfoProvider U>
void Socket<T, U>::bind(AddrInfoList<U> const & target)
{
    AddrInfo const * it = target.cur();
    for (; it != nullptr; it = target.advance()) {
        if (T::SUCCESS == api.bind(sockfd, it->ai_addr, it->ai_addrlen)) {
            return;
        }
    }
    throw api.err_no();
}

template <SocketProvider T, AddrInfoProvider U>
void Socket<T, U>::connect(AddrInfoList<U> const & target)
{
    AddrInfo const * it = target.cur();
    for (; it != nullptr; it = target.advance()) {
        if (T::SUCCESS == api.connect(sockfd, it->ai_addr, it->ai_addrlen)) {
            return;
        }
    }
    throw api.err_no();
}

template <SocketProvider T, AddrInfoProvider U>
void Socket<T, U>::bind(AddrInfo const & target)
{
    if (T::ERROR == api.bind(sockfd, target.ai_addr, target.ai_addrlen)) {
        throw api.err_no();
    }
}

template <SocketProvider T, AddrInfoProvider U>
void Socket<T, U>::connect(AddrInfo const & target)
{
    if (T::ERROR == api.connect(sockfd, target.ai_addr, target.ai_addrlen)) {
        throw api.err_no();
    }
}

template <SocketProvider T, AddrInfoProvider U>
void Socket<T, U>::listen(int backlog)
{
    if (T::ERROR == api.listen(sockfd, backlog)) {
        throw api.err_no();
    }
}

template <SocketProvider T, AddrInfoProvider U>
auto Socket<T, U>::accept(SockInfo<T>& sockinfo) -> Socket<T, U>
{
    int const fd = api.accept(sockfd, &(sockinfo.addr), &(sockinfo.addrlen));
    if (NULL_FD == fd) { throw api.err_no(); }
    return Socket{api, fd};
}

}  // namespace alewa::net
