#pragma once

#include <memory>
#include <cassert>

#include "netapi.hpp"

namespace alewa::net {

template <PosixNetworkApi T>
class AddrInfoList
{
private:
    using AddrInfo = typename T::AddrInfo;

    std::unique_ptr<AddrInfo, typename T::AiDeleter> head;
    AddrInfo const * iter;

public:
    AddrInfoList(T const & api, char const * node, char const * service,
                 AddrInfo const * hints)
            : head(nullptr, api.freeaddrinfo)
    {
        AddrInfo* l = nullptr;
        int ret = api.getaddrinfo(node, service, hints, &l);
        if (ret != T::SUCCESS) {
            std::string m = "getaddrinfo: ";
            throw std::runtime_error{m + api.gai_strerror(ret)};
        }
        head.reset(l);
        iter = l;
    }

    auto current() const noexcept -> AddrInfo const * { return iter; }

    auto advance() noexcept -> AddrInfo const *
    {
        assert(iter);
        iter = iter->ai_next;
        return iter;
    }
};

template <PosixNetworkApi T>
struct SockInfo
{
    typename T::SockAddr addr;
    typename T::SockLen addrlen;
};

template <PosixNetworkApi T>
class Socket
{
private:
    using AddrInfo = typename T::AddrInfo;

    static int const NULL_FD = T::ERROR;
    T const & api;
    int sockfd;

public:
    Socket(T const & api, AddrInfoList<T>& spec);
    ~Socket();

    Socket(Socket&) = delete;
    Socket& operator=(Socket&) = delete;

    Socket(Socket&& other) noexcept;
    Socket& operator=(Socket&& other) noexcept;

    void bind(AddrInfo const & target);
    void connect(AddrInfo const & target);

    void listen(int backlog);
    auto accept(SockInfo<T>& client_info) -> Socket;

    void set_option(int level, int optname, int optval);
    void fcntl(int cmd, int arg);

    [[nodiscard]]
    auto fd() const noexcept -> int { return sockfd; }

private:
    Socket(T const & api, int sockfd) : api(api), sockfd(sockfd) {};
    auto err_msg(std::string const & func) -> std::string;
};

template <PosixNetworkApi T>
Socket<T>::Socket(T const & api, AddrInfoList<T>& spec) : api(api)
{
    AddrInfo const * it = spec.current();
    for (; it != nullptr; it = spec.advance()) {
        sockfd = api.socket(it->ai_family, it->ai_socktype, it->ai_protocol);
        if (sockfd != NULL_FD) { return; }
    }
    throw std::runtime_error{"socket constructor: " + api.error()};
}

template <PosixNetworkApi T>
Socket<T>::~Socket()
{
    if (sockfd == NULL_FD) { return; }
    api.close(sockfd); /* TODO: stderr if this fails */
}

template <PosixNetworkApi T>
Socket<T>::Socket(Socket&& other) noexcept
        : api(other.api), sockfd(other.sockfd)
{
    other.sockfd = NULL_FD;
}

template <PosixNetworkApi T>
auto Socket<T>::operator=(Socket&& other) noexcept -> Socket<T>&
{
    std::swap(sockfd, other.sockfd);
    return *this;
}

template <PosixNetworkApi T>
void Socket<T>::bind(AddrInfo const & target)
{
    if (T::ERROR == api.bind(sockfd, target.ai_addr, target.ai_addrlen)) {
        throw std::runtime_error{err_msg(__func__)};
    }
}

template <PosixNetworkApi T>
void Socket<T>::connect(AddrInfo const & target)
{
    if (T::ERROR == api.connect(sockfd, target.ai_addr, target.ai_addrlen)) {
        throw std::runtime_error{err_msg(__func__)};
    }
}

template <PosixNetworkApi T>
void Socket<T>::listen(int backlog)
{
    if (T::ERROR == api.listen(sockfd, backlog)) {
        throw std::runtime_error{err_msg(__func__)};
    }
}

template <PosixNetworkApi T>
auto Socket<T>::accept(SockInfo<T>& client_info) -> Socket<T>
{
    int const fd = api.accept(sockfd, &client_info.addr, &client_info.addrlen);
    if (fd == NULL_FD) {
        throw std::runtime_error{err_msg(__func__)};
    }
    return Socket{api, fd};
}

template <PosixNetworkApi T>
void Socket<T>::set_option(int level, int optname, int const optval)
{
    if (T::ERROR == api.setsockopt(sockfd, level, optname,
                                   &optval, sizeof(optval))) {
        throw std::runtime_error{err_msg(__func__)};
    }
}

template <PosixNetworkApi T>
void Socket<T>::fcntl(int cmd, int arg)
{
    if (T::ERROR == api.fcntl(sockfd, cmd, arg)) {
        throw std::runtime_error{err_msg(__func__)};
    }
}

template <PosixNetworkApi T>
auto Socket<T>::err_msg(std::string const & func)
        -> std::string
{
    return func + " on socket " + std::to_string(sockfd) + ": " + api.error();
}

}  // namespace alewa::net
