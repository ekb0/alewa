#include "sysnetapi.hpp"

namespace alewa::net::details {

char const * neterror() { return strerror(errno); }

}  // namespace alewa::net::details
