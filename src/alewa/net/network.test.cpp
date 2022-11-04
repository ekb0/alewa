#include "test/test.hpp"

#include "network.hpp"
#include "mocknetapi.hpp"

namespace alewa::net::test {

TEST(addrinfolist_construction)
{
    MockAddrInfoProvider api;
    AddrInfoList happy{api, nullptr, nullptr, nullptr};
    EXPECT_EQ(happy.cur(), &api.ai);

    try {
        api.ret_code = MockAddrInfoProvider::ERROR;
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
    MockAddrInfoProvider api;
    bool is_freed = false;
    MockAddrInfoProvider::set_is_freed(&is_freed);
    {
        AddrInfoList spec{api, nullptr, nullptr, nullptr};
        EXPECT_EQ(is_freed, false);
    }
    MockAddrInfoProvider::set_is_freed(nullptr);
    EXPECT_EQ(is_freed, true);
}

TEST(socket_construction)
{
    MockAddrInfoProvider ai_api;
    MockSocketProvider sock_api;
    AddrInfoList spec{ai_api, nullptr, nullptr, nullptr};

    Socket happy{sock_api, spec};
    EXPECT_EQ(happy.fd(), sock_api.ret_code);

    try {
        sock_api.ret_code = MockSocketProvider::ERROR;
        Socket sad{sock_api, spec};
    }
    catch (int e) {
        EXPECT_EQ(e, sock_api.errorno);
        return;
    }
    BUG(std::string("sad construction did not throw"));
}

TEST(socket_resource_cleanup)
{
    MockAddrInfoProvider ai_api;
    MockSocketProvider sock_api;
    AddrInfoList spec{ai_api, nullptr, nullptr, nullptr};

    bool is_closed = false;
    MockSocketProvider::set_is_closed(&is_closed);
    {
        Socket socket{sock_api, spec};
        EXPECT_EQ(is_closed, false);
    }
    EXPECT_EQ(is_closed, true);
    MockSocketProvider::set_is_closed(nullptr);
}

TEST(socket_move)
{
    MockAddrInfoProvider ai_api;
    MockSocketProvider sock_api;
    AddrInfoList spec{ai_api, nullptr, nullptr, nullptr};
    Socket socket1{sock_api, spec};
    Socket socket2{sock_api, spec};

    bool is_closed = false;
    MockSocketProvider::set_is_closed(&is_closed);

    Socket newSocket = std::move(socket1);
    socket1.~Socket();  // NULL_FD
    EXPECT_EQ(is_closed, false);

    newSocket = std::move(socket2);
    socket2.~Socket();  // Has socket1's fd
    EXPECT_EQ(is_closed, true);
    is_closed = false;

    MockSocketProvider::set_is_closed(nullptr);
}

TEST(socket_bind_addrinfolist)
{
    MockSocketProvider sock_api;
    MockAddrInfoProvider ai_api;
    AddrInfoList spec{ai_api, nullptr, nullptr, nullptr};

    Socket happy{sock_api, spec};
    happy.bind(spec);
    try {
        Socket sad{sock_api, spec};
        sock_api.ret_code = MockSocketProvider::ERROR;
        sad.bind(spec);
    }
    catch (int e) {
        EXPECT_EQ(e, sock_api.errorno);
        return;
    }
    BUG(std::string("sad bind did not throw"));
}

TEST(socket_bind)
{
    MockAddrInfoProvider ai_api;
    AddrInfoList spec{ai_api, nullptr, nullptr, nullptr};
    MockSocketProvider sock_api;

    Socket happy{sock_api, spec};
    happy.bind(*spec.cur());
    try {
        Socket sad{sock_api, spec};
        sock_api.ret_code = MockSocketProvider::ERROR;
        sad.bind(*spec.cur());
    }
    catch (int e) {
        EXPECT_EQ(e, sock_api.errorno);
        return;
    }
    BUG(std::string("sad bind did not throw"));
}

TEST(socket_connect_addrinfolist)
{
    MockSocketProvider sock_api;
    MockAddrInfoProvider ai_api;
    AddrInfoList spec{ai_api, nullptr, nullptr, nullptr};

    Socket happy{sock_api, spec};
    happy.connect(spec);
    try {
        Socket sad{sock_api, spec};
        sock_api.ret_code = MockSocketProvider::ERROR;
        sad.connect(spec);
    }
    catch (int e) {
        EXPECT_EQ(e, sock_api.errorno);
        return;
    }
    BUG(std::string("sad connect did not throw"));
}

TEST(socket_connect)
{
    MockAddrInfoProvider ai_api;
    AddrInfoList spec{ai_api, nullptr, nullptr, nullptr};
    MockSocketProvider sock_api;

    Socket happy{sock_api, spec};
    happy.connect(*spec.cur());
    try {
        Socket sad{sock_api, spec};
        sock_api.ret_code = MockSocketProvider::ERROR;
        sad.connect(*spec.cur());
    }
    catch (int e) {
        EXPECT_EQ(e, sock_api.errorno);
        return;
    }
    BUG(std::string("sad connect did not throw"));
}

TEST(socket_listen)
{
    MockSocketProvider sock_api;
    MockAddrInfoProvider ai_api;
    AddrInfoList spec{ai_api, nullptr, nullptr, nullptr};

    Socket happy{sock_api, spec};
    happy.listen(0);
    try {
        Socket sad{sock_api, spec};
        sock_api.ret_code = MockSocketProvider::ERROR;
        sad.listen(0);
    }
    catch (int e) {
        EXPECT_EQ(e, sock_api.errorno);
        return;
    }
    BUG(std::string("sad listen did not throw"));
}

TEST(socket_accept)
{
    MockSocketProvider sock_api;
    MockAddrInfoProvider ai_api;
    AddrInfoList spec{ai_api, nullptr, nullptr, nullptr};
    Socket sock{sock_api, spec};

    sockaddr addr = {0xB00, {0,69,2}};
    sock_api.ai.ai_addr = &addr;

    SockInfo<MockSocketProvider> happy_info{};
    Socket happy = sock.accept(happy_info);

    EXPECT_EQ(happy_info.addr.sa_family, addr.sa_family);
    EXPECT_EQ(happy_info.addr.sa_data[1], addr.sa_data[1]);

    try {
        SockInfo<MockSocketProvider> sad_info{};
        sock_api.ret_code = MockSocketProvider::ERROR;
        sock.accept(sad_info);
    }
    catch(int e) {
        EXPECT_EQ(e, sock_api.errorno);
        return;
    }
    BUG(std::string("sad accept did not throw"));
}

}  // namespace alewa::net::test
