#include "err_desc.hpp"

#include <cstring>

namespace alewa::sys {

std::string SystemErrorDescription::error() const
{
    return ::strerror(errno);
}

}  // namespace alewa::sys
