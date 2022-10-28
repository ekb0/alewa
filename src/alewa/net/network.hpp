#pragma once

#include <string>
#include <memory>
#include <sstream>

#include "netapi.hpp"

namespace alewa::net {

template <NetApi T>
class AddrInfo
{
    using Ai = typename T::addrinfo;

private:
    std::unique_ptr<Ai, typename T::freeaddrinfo_t> ai_list;

public:
    AddrInfo(T api, char const * node, char const * service, Ai const & hints);

    Ai const * list() { return ai_list.get(); }
};

template <NetApi T>
AddrInfo<T>::AddrInfo(T api, char const * node, char const * service,
                      Ai const & hints)
        : ai_list(nullptr, api.freeaddrinfo)
{
    Ai* l;

    int ret = api.getaddrinfo(node, service, &hints, &l);
    if (ret != T::SUCCESS) {
        std::ostringstream err;
        err << "getaddrinfo: " << api.gai_strerror(ret) << "\n";
        throw std::runtime_error(err.str());
    }
    ai_list.reset(l);
}

}  // namespace alewa::net
