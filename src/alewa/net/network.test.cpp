#include "test/test.hpp"

#include "network.hpp"
#include "mocknet_provider.hpp"

namespace alewa::net::test {

TEST(addrinfolist_construction)
{
    MockNetworkProvider api;
    AddrInfoList happy{api, nullptr, nullptr, nullptr};
    EXPECT_EQ(happy.cur(), &api.ai);

    try {
        api.ret_code = MockNetworkProvider::ERROR;
        AddrInfoList sad{api, nullptr, nullptr, nullptr};
    }
    catch (int e) {
        EXPECT_EQ(e, api.ret_code);
        return;
    }
    BUG(std::string("sad construction did not throw"));
}

TEST(addrinfolist_resource_cleanup)
{
    MockNetworkProvider api;
    bool is_freed = false;
    MockNetworkProvider::set_is_freed(&is_freed);
    {
        AddrInfoList spec{api, nullptr, nullptr, nullptr};
        EXPECT_EQ(is_freed, false);
    }
    MockNetworkProvider::set_is_freed(nullptr);
    EXPECT_EQ(is_freed, true);
}

TEST(socket_construction)
{
    MockNetworkProvider api;
    AddrInfoList spec{api, nullptr, nullptr, nullptr};

    Socket happy{api, spec};
    EXPECT_EQ(happy.fd(), api.ret_code);

    try {
        api.ret_code = MockNetworkProvider::ERROR;
        Socket sad{api, spec};
    }
    catch (int e) {
        EXPECT_EQ(e, api.errorno);
        return;
    }
    BUG(std::string("sad construction did not throw"));
}

TEST(socket_resource_cleanup)
{
    MockNetworkProvider api;
    AddrInfoList spec{api, nullptr, nullptr, nullptr};

    bool is_closed = false;
    MockNetworkProvider::set_is_closed(&is_closed);
    {
        Socket socket{api, spec};
        EXPECT_EQ(is_closed, false);
    }
    EXPECT_EQ(is_closed, true);
    MockNetworkProvider::set_is_closed(nullptr);
}

TEST(socket_move)
{
    MockNetworkProvider api;
    AddrInfoList spec{api, nullptr, nullptr, nullptr};
    Socket socket1{api, spec};
    Socket socket2{api, spec};

    bool is_closed = false;
    MockNetworkProvider::set_is_closed(&is_closed);

    Socket newSocket = std::move(socket1);
    socket1.~Socket();  // NULL_FD
    EXPECT_EQ(is_closed, false);

    newSocket = std::move(socket2);
    socket2.~Socket();  // Has socket1's fd
    EXPECT_EQ(is_closed, true);
    is_closed = false;

    MockNetworkProvider::set_is_closed(nullptr);
}

TEST(socket_bind_addrinfolist)
{
    MockNetworkProvider api;
    AddrInfoList spec{api, nullptr, nullptr, nullptr};

    Socket happy{api, spec};
    happy.bind(spec);
    try {
        Socket sad{api, spec};
        api.ret_code = MockNetworkProvider::ERROR;
        sad.bind(spec);
    }
    catch (int e) {
        EXPECT_EQ(e, api.errorno);
        return;
    }
    BUG(std::string("sad bind did not throw"));
}

TEST(socket_bind)
{
    MockNetworkProvider api;
    AddrInfoList spec{api, nullptr, nullptr, nullptr};

    Socket happy{api, spec};
    happy.bind(*spec.cur());
    try {
        Socket sad{api, spec};
        api.ret_code = MockNetworkProvider::ERROR;
        sad.bind(*spec.cur());
    }
    catch (int e) {
        EXPECT_EQ(e, api.errorno);
        return;
    }
    BUG(std::string("sad bind did not throw"));
}

TEST(socket_connect_addrinfolist)
{
    MockNetworkProvider api;
    AddrInfoList spec{api, nullptr, nullptr, nullptr};

    Socket happy{api, spec};
    happy.connect(spec);
    try {
        Socket sad{api, spec};
        api.ret_code = MockNetworkProvider::ERROR;
        sad.connect(spec);
    }
    catch (int e) {
        EXPECT_EQ(e, api.errorno);
        return;
    }
    BUG(std::string("sad connect did not throw"));
}

TEST(socket_connect)
{
    MockNetworkProvider api;
    AddrInfoList spec{api, nullptr, nullptr, nullptr};

    Socket happy{api, spec};
    happy.connect(*spec.cur());
    try {
        Socket sad{api, spec};
        api.ret_code = MockNetworkProvider::ERROR;
        sad.connect(*spec.cur());
    }
    catch (int e) {
        EXPECT_EQ(e, api.errorno);
        return;
    }
    BUG(std::string("sad connect did not throw"));
}

TEST(socket_listen)
{
    MockNetworkProvider api;
    AddrInfoList spec{api, nullptr, nullptr, nullptr};

    Socket happy{api, spec};
    happy.listen(0);
    try {
        Socket sad{api, spec};
        api.ret_code = MockNetworkProvider::ERROR;
        sad.listen(0);
    }
    catch (int e) {
        EXPECT_EQ(e, api.errorno);
        return;
    }
    BUG(std::string("sad listen did not throw"));
}

TEST(socket_accept)
{
    MockNetworkProvider api;
    AddrInfoList spec{api, nullptr, nullptr, nullptr};
    Socket sock{api, spec};

    sockaddr addr = {0xB00, {0,69,2}};
    api.ai.ai_addr = &addr;

    SockInfo<MockNetworkProvider> happy_info{};
    Socket happy = sock.accept(happy_info);

    EXPECT_EQ(happy_info.addr.sa_family, addr.sa_family);
    EXPECT_EQ(happy_info.addr.sa_data[1], addr.sa_data[1]);

    try {
        SockInfo<MockNetworkProvider> sad_info{};
        api.ret_code = MockNetworkProvider::ERROR;
        sock.accept(sad_info);
    }
    catch(int e) {
        EXPECT_EQ(e, api.errorno);
        return;
    }
    BUG(std::string("sad accept did not throw"));
}

}  // namespace alewa::net::test
