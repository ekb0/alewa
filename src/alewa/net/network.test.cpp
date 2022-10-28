#include "test/test.hpp"

#include "network.hpp"
#include "mocknetapi.hpp"

namespace alewa::net::test {

TEST(AddrInfoListConstruction)
{
    MockAddrInfoProvider::addrinfo hints{};

    MockAddrInfoProvider api;
    AddrInfoList<MockAddrInfoProvider> ais{api, nullptr, "3490", hints};

    EXPECT(true);
}

}  // namespace alewa::net::test