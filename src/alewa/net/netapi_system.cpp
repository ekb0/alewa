#include "netapi_system.hpp"

namespace alewa::net {

std::string SystemNetworkApi::error() const
{
    return ::strerror(errno);
}

}  // namespace alewa::net
