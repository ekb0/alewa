#include "test/test.hpp"

#include "network.hpp"
#include "mocknetapi.hpp"

namespace alewa::net::test {

std::string api_err_msg(std::string const & msg)
{
    std::ostringstream ss;
    ss << msg << ": " << MockProviderBase::err << "\n";
    return ss.str();
}

TEST(addrinfolist_construction)
{
    MockAddrInfoProvider api;
    AddrInfoList happy{api, nullptr, nullptr, nullptr};
    EXPECT_EQ(happy.first(), &api.ai);

    try {
        api.ret_code = MockAddrInfoProvider::ERROR;
        AddrInfoList sad{api, nullptr, nullptr, nullptr};
    }
    catch (std::runtime_error const & e) {
        EXPECT_EQ(e.what(), api_err_msg("getaddrinfo"));
        return;
    }
    BUG(std::string("sad construction did not throw"));
}

TEST(addrinfolist_resource_cleanup)
{
    bool is_freed = false;
    MockAddrInfoProvider api;
    MockAddrInfoProvider::set_is_freed(&is_freed);
    {
        AddrInfoList ais{api, nullptr, nullptr, nullptr};
        EXPECT_EQ(is_freed, false);
    }
    MockAddrInfoProvider::set_is_freed(nullptr);
    EXPECT_EQ(is_freed, true);
}

TEST(socket_construction)
{
    MockAddrInfoProvider ai_api;
    ai_api.ai.ai_flags = 0xB00B5; // hehe
    AddrInfoList ais{ai_api, nullptr, nullptr, nullptr};
    MockSocketProvider sock_api;

    Socket happy{sock_api, ais};
    EXPECT_EQ(happy.fd(), sock_api.ret_code);
    EXPECT_EQ(happy.info().ai_flags, ai_api.ai.ai_flags);

    try {
        sock_api.ret_code = MockSocketProvider::ERROR;
        Socket sad{sock_api, ais};
    }
    catch (std::runtime_error const & e) {
        EXPECT_EQ(e.what(), api_err_msg("socket"));
        return;
    }
    BUG(std::string("sad construction did not throw"));
}

TEST(socket_resource_cleanup)
{
    MockAddrInfoProvider ai_api;
    AddrInfoList ais{ai_api, nullptr, nullptr, nullptr};
    bool is_closed = false;
    MockSocketProvider sock_api;

    MockSocketProvider::set_is_closed(&is_closed);
    {
        Socket socket{sock_api, ais};
        EXPECT_EQ(is_closed, false);
    }
    EXPECT_EQ(is_closed, true);
    MockSocketProvider::set_is_closed(nullptr);
}

auto new_sock(MockSocketProvider const & sock_api,
              MockAddrInfoProvider const & ai_api)
{
    AddrInfoList ais{ai_api, nullptr, nullptr, nullptr};
    return Socket{sock_api, ais};
}

TEST(socket_bind)
{
    MockSocketProvider sock_api;
    MockAddrInfoProvider ai_api;
    Socket happy = new_sock(sock_api, ai_api);
    happy.bind(sock_api);

    try {
        Socket sad = new_sock(sock_api, ai_api);
        sock_api.ret_code = MockSocketProvider::ERROR;
        sad.bind(sock_api);
    }
    catch(std::runtime_error const & e) {
        EXPECT_EQ(e.what(), api_err_msg("bind"));
        return;
    }
    BUG(std::string("sad bind did not throw"));
}

TEST(socket_connect)
{
    MockSocketProvider sock_api;
    MockAddrInfoProvider ai_api;
    Socket happy = new_sock(sock_api, ai_api);
    happy.connect(sock_api);
    try {
        Socket sad = new_sock(sock_api, ai_api);
        sock_api.ret_code = MockSocketProvider::ERROR;
        sad.connect(sock_api);
    }
    catch(std::runtime_error const & e) {
        EXPECT_EQ(e.what(), api_err_msg("connect"));
        return;
    }
    BUG(std::string("sad connect did not throw"));
}

TEST(socket_listen)
{
    MockSocketProvider sock_api;
    MockAddrInfoProvider ai_api;
    Socket happy = new_sock(sock_api, ai_api);
    happy.listen(sock_api, 0);
    try {
        Socket sad = new_sock(sock_api, ai_api);
        sock_api.ret_code = MockSocketProvider::ERROR;
        sad.listen(sock_api, 0);
    }
    catch(std::runtime_error const & e) {
        EXPECT_EQ(e.what(), api_err_msg("listen"));
        return;
    }
    BUG(std::string("sad listen did not throw"));
}

TEST(socket_accept)
{
    MockSocketProvider sock_api;
    MockAddrInfoProvider ai_api;
    sockaddr addr = {0xB00, {0,69,2}};
    sock_api.ai.ai_addr = &addr;
    Socket sock = new_sock(sock_api, ai_api);

    Socket happy = sock.accept(sock_api);
    EXPECT_EQ(happy.info().ai_addr->sa_family, addr.sa_family);
    EXPECT_EQ(happy.info().ai_addr->sa_data[1], addr.sa_data[1]);

    try {
        sock_api.ret_code = MockSocketProvider::ERROR;
        sock.accept(sock_api);
    }
    catch(int e) {
        EXPECT_EQ(e, MockSocketProvider::ERROR);
        return;
    }
    BUG(std::string("sad accept did not throw"));
}

}  // namespace alewa::net::test
