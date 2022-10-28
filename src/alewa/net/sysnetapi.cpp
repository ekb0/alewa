#include "sysnetapi.hpp"

namespace alewa::net {

char const * SysNetApi::neterror() const
{
    return strerror(errno);
}

}  // namespace alewa::net
