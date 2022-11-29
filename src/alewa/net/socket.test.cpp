#include "test/test.hpp"

#include "socket.hpp"
#include "netapi_mock.hpp"

namespace alewa::net::test {

TEST(addrinfolist_construction)
{
    MockNetworkApi api;
    AddrInfoList<MockNetworkApi> happy{api, nullptr, nullptr, nullptr};
    EXPECT_EQ(happy.current(), &api.ai);

    try {
        api.ret_code = MockNetworkApi::ERROR;
        AddrInfoList<MockNetworkApi> sad{api, nullptr, nullptr, nullptr};
    }
    catch (std::runtime_error const & e) {
        EXPECT_EQ(std::string{e.what()}, api.gai_strerror(api.ret_code));
        return;
    }
    BUG(std::string{"sad construction did not throw"});
}

TEST(addrinfolist_resource_cleanup)
{
    MockNetworkApi api;
    bool is_freed = false;
    MockNetworkApi::set_is_freed(&is_freed);
    {
        AddrInfoList<MockNetworkApi> spec{api, nullptr, nullptr, nullptr};
        EXPECT_EQ(is_freed, false);
    }
    MockNetworkApi::set_is_freed(nullptr);
    EXPECT_EQ(is_freed, true);
}

TEST(socket_construction)
{
    MockNetworkApi api;
    AddrInfoList<MockNetworkApi> spec{api, nullptr, nullptr, nullptr};

    Socket<MockNetworkApi> happy{api, spec};
    EXPECT_EQ(happy.fd(), api.ret_code);

    try {
        api.ret_code = MockNetworkApi::ERROR;
        Socket<MockNetworkApi> sad{api, spec};
    }
    catch (std::runtime_error const & e) {
        EXPECT_EQ(e.what(), api.error());
        return;
    }
    BUG(std::string{"sad construction did not throw"});
}

TEST(socket_resource_cleanup)
{
    MockNetworkApi api;
    AddrInfoList<MockNetworkApi> spec{api, nullptr, nullptr, nullptr};

    bool is_closed = false;
    MockNetworkApi::set_is_closed(&is_closed);
    {
        Socket<MockNetworkApi> socket{api, spec};
        EXPECT_EQ(is_closed, false);
    }
    EXPECT_EQ(is_closed, true);
    MockNetworkApi::set_is_closed(nullptr);
}

TEST(socket_move)
{
    MockNetworkApi api;
    AddrInfoList<MockNetworkApi> spec{api, nullptr, nullptr, nullptr};
    Socket<MockNetworkApi> socket1{api, spec};
    Socket<MockNetworkApi> socket2{api, spec};

    bool is_closed = false;
    MockNetworkApi::set_is_closed(&is_closed);

    Socket<MockNetworkApi> newSocket = std::move(socket1);
    socket1.~Socket<MockNetworkApi>();  // NULL_FD
    EXPECT_EQ(is_closed, false);

    newSocket = std::move(socket2);
    socket2.~Socket<MockNetworkApi>();  // Has socket1's fd
    EXPECT_EQ(is_closed, true);
    is_closed = false;

    MockNetworkApi::set_is_closed(nullptr);
}

TEST(socket_bind_addrinfolist)
{
    MockNetworkApi api;
    AddrInfoList<MockNetworkApi> spec{api, nullptr, nullptr, nullptr};

    Socket<MockNetworkApi> happy{api, spec};
    happy.bind(spec);
    try {
        Socket<MockNetworkApi> sad{api, spec};
        api.ret_code = MockNetworkApi::ERROR;
        sad.bind(spec);
    }
    catch (std::runtime_error const & e) {
        EXPECT_EQ(e.what(), api.error());
        return;
    }
    BUG(std::string{"sad bind did not throw"});
}

TEST(socket_bind)
{
    MockNetworkApi api;
    AddrInfoList<MockNetworkApi> spec{api, nullptr, nullptr, nullptr};

    Socket<MockNetworkApi> happy{api, spec};
    happy.bind(*spec.current());
    try {
        Socket<MockNetworkApi> sad{api, spec};
        api.ret_code = MockNetworkApi::ERROR;
        sad.bind(*spec.current());
    }
    catch (std::runtime_error const & e) {
        EXPECT_EQ(e.what(), api.error());
        return;
    }
    BUG(std::string{"sad bind did not throw"});
}

TEST(socket_connect_addrinfolist)
{
    MockNetworkApi api;
    AddrInfoList<MockNetworkApi> spec{api, nullptr, nullptr, nullptr};

    Socket<MockNetworkApi> happy{api, spec};
    happy.connect(spec);
    try {
        Socket<MockNetworkApi> sad{api, spec};
        api.ret_code = MockNetworkApi::ERROR;
        sad.connect(spec);
    }
    catch (std::runtime_error const & e) {
        EXPECT_EQ(e.what(), api.error());
        return;
    }
    BUG(std::string{"sad connect did not throw"});
}

TEST(socket_connect)
{
    MockNetworkApi api;
    AddrInfoList<MockNetworkApi> spec{api, nullptr, nullptr, nullptr};

    Socket<MockNetworkApi> happy{api, spec};
    happy.connect(*spec.current());
    try {
        Socket<MockNetworkApi> sad{api, spec};
        api.ret_code = MockNetworkApi::ERROR;
        sad.connect(*spec.current());
    }
    catch (std::runtime_error const & e) {
        EXPECT_EQ(e.what(), api.error());
        return;
    }
    BUG(std::string{"sad connect did not throw"});
}

TEST(socket_listen)
{
    MockNetworkApi api;
    AddrInfoList<MockNetworkApi> spec{api, nullptr, nullptr, nullptr};

    Socket<MockNetworkApi> happy{api, spec};
    happy.listen(0);
    try {
        Socket<MockNetworkApi> sad{api, spec};
        api.ret_code = MockNetworkApi::ERROR;
        sad.listen(0);
    }
    catch (std::runtime_error const & e) {
        EXPECT_EQ(e.what(), api.error());
        return;
    }
    BUG(std::string{"sad listen did not throw"});
}

TEST(socket_accept)
{
    MockNetworkApi api;
    AddrInfoList<MockNetworkApi> spec{api, nullptr, nullptr, nullptr};
    Socket<MockNetworkApi> sock{api, spec};

    sockaddr addr = {0xB00, {0,69,2}};
    api.ai.ai_addr = &addr;

    SockInfo<MockNetworkApi> happy_info{};
    Socket<MockNetworkApi> happy = sock.accept(happy_info);

    EXPECT_EQ(happy_info.addr.sa_family, addr.sa_family);
    EXPECT_EQ(happy_info.addr.sa_data[1], addr.sa_data[1]);

    try {
        SockInfo<MockNetworkApi> sad_info{};
        api.ret_code = MockNetworkApi::ERROR;
        sock.accept(sad_info);
    }
    catch (std::runtime_error const & e) {
        EXPECT_EQ(e.what(), api.error());
        return;
    }
    BUG(std::string{"sad accept did not throw"});
}

}  // namespace alewa::net::test
