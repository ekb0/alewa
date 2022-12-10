#include "ioapi_sys.hpp"

#include <cstring>

namespace alewa::io {

    std::string SysErrorDescription::error() const
    {
        return ::strerror(errno);
    }

}  // namespace alewa::io
