#pragma once

#include <memory>
#include <cassert>

#include "ioapi.hpp"

namespace alewa::io {

template <SocketApi T>
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
        AddrInfo* ai_list = nullptr;
        int ret = api.getaddrinfo(node, service, hints, &ai_list);
        if (ret != T::SUCCESS) {
            std::string m = "getaddrinfo: ";
            throw std::runtime_error{m + api.gai_strerror(ret)};
        }
        head.reset(ai_list);
        iter = ai_list;
    }

    auto current() const noexcept -> AddrInfo const * { return iter; }

    auto advance() noexcept -> AddrInfo const *
    {
        assert(iter);
        iter = iter->ai_next;
        return iter;
    }
};

template <SocketApi T>
struct SockInfo
{
    typename T::SockAddr addr;
    typename T::SockLen addrlen;
};

template <SocketApi T>
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

    auto operator<=>(Socket const & other) const -> int;
    auto operator==(Socket const & other) const -> bool;

    [[nodiscard]]
    auto fd() const noexcept -> int { return sockfd; }

    void bind(AddrInfo const & target);
    void connect(AddrInfo const & target);

    void listen(int backlog);
    auto accept(SockInfo<T>& client_info) -> Socket;

    void set_file_option(int cmd, int arg);
    void set_socket_option(int level, int optname, int optval);

private:
    Socket(T const & api, int sockfd) : api(api), sockfd(sockfd) {};
    auto err_msg(std::string const & func) -> std::string;
};

}  // namespace alewa::io

template<alewa::io::SocketApi T>
struct std::hash<alewa::io::Socket<T>>
{
    auto operator()(alewa::io::Socket<T> const& s) const noexcept
            -> std::size_t
    {
        return std::hash<int>{}(s.fd());
    }
};


namespace alewa::io {

template <SocketApi T>
Socket<T>::Socket(T const & api, AddrInfoList<T>& spec) : api(api)
{
    AddrInfo const * it = spec.current();
    for (; it != nullptr; it = spec.advance()) {
        sockfd = api.socket(it->ai_family, it->ai_socktype, it->ai_protocol);
        if (sockfd != NULL_FD) { return; }
    }
    throw std::runtime_error{"socket constructor: " + api.error()};
}

template <SocketApi T>
Socket<T>::~Socket()
{
    if (sockfd == NULL_FD) { return; }
    api.close(sockfd); /* TODO: stderr if this fails */
}

template <SocketApi T>
Socket<T>::Socket(Socket&& other) noexcept
        : api(other.api), sockfd(other.sockfd)
{
    other.sockfd = NULL_FD;
}

template <SocketApi T>
auto Socket<T>::operator=(Socket&& other) noexcept -> Socket<T>&
{
    std::swap(sockfd, other.sockfd);
    return *this;
}

template <SocketApi T>
auto Socket<T>::operator<=>(Socket const & other) const -> int
{
    return sockfd - other.sockfd;
}

template <SocketApi T>
auto Socket<T>::operator==(Socket const & other) const -> bool
{
    return sockfd == other.sockfd;
}

template <SocketApi T>
void Socket<T>::bind(AddrInfo const & target)
{
    if (T::ERROR == api.bind(sockfd, target.ai_addr, target.ai_addrlen)) {
        throw std::runtime_error{err_msg(__func__)};
    }
}

template <SocketApi T>
void Socket<T>::connect(AddrInfo const & target)
{
    if (T::ERROR == api.connect(sockfd, target.ai_addr, target.ai_addrlen)) {
        throw std::runtime_error{err_msg(__func__)};
    }
}

template <SocketApi T>
void Socket<T>::listen(int backlog)
{
    if (T::ERROR == api.listen(sockfd, backlog)) {
        throw std::runtime_error{err_msg(__func__)};
    }
}

template <SocketApi T>
auto Socket<T>::accept(SockInfo<T>& client_info) -> Socket<T>
{
    int const fd = api.accept(sockfd, &client_info.addr, &client_info.addrlen);
    if (fd == NULL_FD) {
        throw std::runtime_error{err_msg(__func__)};
    }
    return Socket{api, fd};
}

template <SocketApi T>
void Socket<T>::set_file_option(int cmd, int arg)
{
    if (T::ERROR == api.fcntl(sockfd, cmd, arg)) {
        throw std::runtime_error{err_msg(__func__)};
    }
}

template <SocketApi T>
void Socket<T>::set_socket_option(int level, int optname, int const optval)
{
    if (T::ERROR == api.setsockopt(sockfd, level, optname,
                                   &optval, sizeof(optval))) {
        throw std::runtime_error{err_msg(__func__)};
    }
}

template <SocketApi T>
auto Socket<T>::err_msg(std::string const & func) -> std::string
{
    return func + " on socket " + std::to_string(sockfd) + ": " + api.error();
}

}  // namespace alewa::io
