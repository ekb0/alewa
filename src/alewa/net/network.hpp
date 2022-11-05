#pragma once

#include <memory>
#include <cassert>

#include "net_provider.hpp"

namespace alewa::net {

template <NetworkProvider T>
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

template <NetworkProvider T>
struct SockInfo
{
    typename T::SockAddr addr;
    typename T::SockLen_t addrlen;
};

template <NetworkProvider T>
class Socket
{
private:
    using AddrInfo = typename T::AddrInfo;

    static int const NULL_FD = T::ERROR;
    T const & api;
    int sockfd;

    Socket(T const & api, int sockfd) : api(api), sockfd(sockfd) {};

public:
    Socket(T const & api, AddrInfoList<T>& spec);
    ~Socket();

    Socket(Socket&) = delete;
    Socket& operator=(Socket&) = delete;

    Socket(Socket&& other) noexcept;
    Socket& operator=(Socket&& other) noexcept;

    void bind(AddrInfoList<T>& target);
    void connect(AddrInfoList<T>& target);

    void bind(AddrInfo const & target);
    void connect(AddrInfo const & target);

    void listen(int backlog);
    auto accept(SockInfo<T>& sockinfo) -> Socket;

    [[nodiscard]] auto fd() const noexcept -> int { return sockfd; }
};

template <NetworkProvider T>
Socket<T>::Socket(T const & api, AddrInfoList<T>& spec) : api(api)
{
    AddrInfo const * it = spec.cur();
    for (; it != nullptr; it = spec.advance()) {
        sockfd = api.socket(it->ai_family, it->ai_socktype, it->ai_protocol);
        if (NULL_FD != sockfd) { return; }
    }
    throw api.err_no();
}

template <NetworkProvider T>
Socket<T>::~Socket()
{
    if (NULL_FD == sockfd) { return; }
    api.close(sockfd); /* TODO: stderr if this fails */
}

template <NetworkProvider T>
Socket<T>::Socket(Socket&& other) noexcept
        : api(other.api), sockfd(other.sockfd)
{
    other.sockfd = NULL_FD;
}

template <NetworkProvider T>
auto Socket<T>::operator=(Socket&& other) noexcept -> Socket<T>&
{
    assert(&api == &other.api);
    std::swap(sockfd, other.sockfd);
    return *this;
}

template <NetworkProvider T>
void Socket<T>::bind(AddrInfoList<T>& target)
{
    AddrInfo const * it = target.cur();
    for (; it != nullptr; it = target.advance()) {
        if (T::SUCCESS == api.bind(sockfd, it->ai_addr, it->ai_addrlen)) {
            return;
        }
    }
    throw api.err_no();
}

template <NetworkProvider T>
void Socket<T>::connect(AddrInfoList<T>& target)
{
    AddrInfo const * it = target.cur();
    for (; it != nullptr; it = target.advance()) {
        if (T::SUCCESS == api.connect(sockfd, it->ai_addr, it->ai_addrlen)) {
            return;
        }
    }
    throw api.err_no();
}

template <NetworkProvider T>
void Socket<T>::bind(AddrInfo const & target)
{
    if (T::ERROR == api.bind(sockfd, target.ai_addr, target.ai_addrlen)) {
        throw api.err_no();
    }
}

template <NetworkProvider T>
void Socket<T>::connect(AddrInfo const & target)
{
    if (T::ERROR == api.connect(sockfd, target.ai_addr, target.ai_addrlen)) {
        throw api.err_no();
    }
}

template <NetworkProvider T>
void Socket<T>::listen(int backlog)
{
    if (T::ERROR == api.listen(sockfd, backlog)) {
        throw api.err_no();
    }
}

template <NetworkProvider T>
auto Socket<T>::accept(SockInfo<T>& sockinfo) -> Socket<T>
{
    int const fd = api.accept(sockfd, &(sockinfo.addr), &(sockinfo.addrlen));
    if (NULL_FD == fd) { throw api.err_no(); }
    return Socket{api, fd};
}

}  // namespace alewa::net
