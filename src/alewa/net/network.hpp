#pragma once

#include <string>
#include <memory>
#include <sstream>

#include "netapi.hpp"

namespace alewa::net {

template <NetApi NApi>
class AddrInfo
{
private:
    std::unique_ptr<struct addrinfo, decltype(&NApi::freeaddrinfo)> ai_list;

public:
    AddrInfo(std::string const & node, std::string const & service,
             struct addrinfo const & hints);

    struct addrinfo const * list() { return ai_list.get(); }
};

template <NetApi NApi>
AddrInfo<NApi>::AddrInfo(std::string const & node, std::string const & service,
                         struct addrinfo const & hints)
        : ai_list(nullptr, &NApi::freeaddrinfo)
{
    struct addrinfo* l;

    int ret = NApi::getaddrinfo(node.c_str(), service.c_str(), &hints, &l);
    if (ret != 0) {
        std::ostringstream err;
        err << "getaddrinfo: " << NApi::gai_strerror(ret) << "\n";
        throw std::runtime_error(err.str());
    }
    ai_list.reset(l);
}

}  // namespace alewa::net
