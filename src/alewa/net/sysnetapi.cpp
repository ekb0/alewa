#include "sysnetapi.hpp"

namespace alewa::net {

auto SysNetApi::error(int err) const -> std::string
{
    return strerror(err);
}

[[nodiscard]] int SysNetApi::err_no() const { return errno; }

}  // namespace alewa::net
