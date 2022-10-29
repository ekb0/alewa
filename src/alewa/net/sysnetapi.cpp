#include "sysnetapi.hpp"

namespace alewa::net {

auto SysNetApi::neterror() const -> char const *
{
    return strerror(errno);
}

}  // namespace alewa::net
