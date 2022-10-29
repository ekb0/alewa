#include "test/test.hpp"

#include "network.hpp"
#include "mocknetapi.hpp"

namespace alewa::net::test {

MockAddrInfoProvider::addrinfo const hints{};

std::string api_err_msg(std::string const & msg)
{
    std::ostringstream ss;
    ss << msg << ": " << MockProviderBase::err << "\n";
    return ss.str();
}

TEST(addrinfolist_happy_construction)
{
    try {
        MockAddrInfoProvider api;
        AddrInfoList ais{api, nullptr, "8080", hints};

        EXPECT_EQ(ais.first(), api.ai_list);
    }
    catch (std::runtime_error const & e) {
        BUG(std::string("unexpected exception: ") + e.what());
    }
}

TEST(addrinfolist_sad_construction)
{
    try {
        MockAddrInfoProvider api;
        api.ret_code = MockAddrInfoProvider::ERROR;
        AddrInfoList ais{api, nullptr, "8080", hints};
    }
    catch (std::runtime_error const & e) {
        EXPECT_EQ(e.what(), api_err_msg("getaddrinfo"));
        return;
    }
    BUG(std::string("unexpected success"));
}

TEST(addrinfolist_resource_cleanup)
{
    MockAddrInfoProvider api;
    MockAddrInfoProvider::nfreed = 0;
    {
        AddrInfoList ais{api, nullptr, "8080", hints};
    }
    EXPECT_EQ(MockAddrInfoProvider::nfreed, 1);
}

TEST(socket_happy_construction)
{
    try {
        MockAddrInfoProvider ai_api;
        ai_api.ai.ai_flags = 0xB00B5; // hehe
        AddrInfoList ais{ai_api, nullptr, "8080", hints};

        MockSocketProvider sock_api;
        Socket socket{sock_api, ais};

        EXPECT_EQ(socket.fd(), sock_api.ret_code);
        EXPECT_NEQ(&(socket.info()), &(ai_api.ai));
        EXPECT_EQ(socket.info().ai_flags, ai_api.ai.ai_flags);
    }
    catch (std::runtime_error const & e) {
        BUG(std::string("unexpected exception: ") + e.what());
    }
}

TEST(socket_sad_construction)
{
    try {
        MockAddrInfoProvider ai_api;
        AddrInfoList ais{ai_api, nullptr, "8080", hints};

        MockSocketProvider sock_api;
        sock_api.ret_code = MockSocketProvider::ERROR;
        Socket socket{sock_api, ais};
    }
    catch (std::runtime_error const & e) {
        EXPECT_EQ(e.what(), api_err_msg("socket"));
        return;
    }
    BUG(std::string("unexpected success"));
}

TEST(socket_resource_cleanup)
{
    MockAddrInfoProvider ai_api;
    MockSocketProvider sock_api;
    AddrInfoList ais{ai_api, nullptr, "8080", hints};

    MockSocketProvider::nclosed = 0;
    {
        Socket socket{sock_api, ais};
    }
    EXPECT_EQ(MockSocketProvider::nclosed, 1);
}

auto new_sock(MockSocketProvider const & sock_api,
              MockAddrInfoProvider const & ai_api)
{
    AddrInfoList ais{ai_api, nullptr, "8080", hints};
    return Socket{sock_api, ais};
}

TEST(socket_happy_bind)
{
    MockSocketProvider sock_api;
    MockAddrInfoProvider ai_api;
    try {
        Socket socket = new_sock(sock_api, ai_api);
        socket.bind();
    }
    catch(std::runtime_error const & e) {
        BUG(std::string("unexpected exception: ") + e.what());
    }
}

TEST(socket_sad_bind)
{
    MockSocketProvider sock_api;
    MockAddrInfoProvider ai_api;
    try {
        Socket socket = new_sock(sock_api, ai_api);
        sock_api.ret_code = MockSocketProvider::ERROR;
        socket.bind();
    }
    catch(std::runtime_error const & e) {
        EXPECT_EQ(e.what(), api_err_msg("bind"));
        return;
    }
    BUG(std::string("unexpected success"));
}

TEST(socket_happy_connect)
{
    MockSocketProvider sock_api;
    MockAddrInfoProvider ai_api;
    try {
        Socket socket = new_sock(sock_api, ai_api);
        socket.connect();
    }
    catch(std::runtime_error const & e) {
        BUG(std::string("unexpected exception: ") + e.what());
    }
}

TEST(socket_sad_connect)
{
    MockSocketProvider sock_api;
    MockAddrInfoProvider ai_api;
    try {
        Socket socket = new_sock(sock_api, ai_api);
        sock_api.ret_code = MockSocketProvider::ERROR;
        socket.connect();
    }
    catch(std::runtime_error const & e) {
        EXPECT_EQ(e.what(), api_err_msg("connect"));
        return;
    }
    BUG(std::string("unexpected success"));
}

}  // namespace alewa::net::test
