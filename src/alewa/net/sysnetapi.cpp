#include "sysnetapi.hpp"

namespace alewa::net {

char const * SysNetApi::neterror()
{
    return strerror(errno);
}

}  // namespace alewa::net
