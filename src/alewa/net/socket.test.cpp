#include "test/test_utils.hpp"

#include "socket.hpp"
#include "netapi_mock.hpp"

namespace alewa::net::test {

auto err_msg(std::string const & func, std::string const & msg)
{
    return func + " on socket 0: " + msg;
}

ALW_TEST(addrinfolist_construction)
{
    MockNetworkApi api;
    AddrInfoList<MockNetworkApi> happy{api, nullptr, nullptr, nullptr};
    ALW_EXPECT_EQ(happy.current(), &api.ai);

    std::string error{};
    try {
        api.ret_code = MockNetworkApi::ERROR;
        AddrInfoList<MockNetworkApi> sad{api, nullptr, nullptr, nullptr};
    }
    catch (std::runtime_error const & e) {
        error = e.what();
    }
    std::string m = std::string{"getaddrinfo: "};
    ALW_EXPECT_EQ(error, m + api.gai_strerror(api.ret_code));
}

ALW_TEST(addrinfolist_resource_cleanup)
{
    MockNetworkApi api;
    bool is_freed = false;
    MockNetworkApi::set_is_freed(&is_freed);
    {
        AddrInfoList<MockNetworkApi> spec{api, nullptr, nullptr, nullptr};
        ALW_EXPECT_EQ(is_freed, false);
    }
    MockNetworkApi::set_is_freed(nullptr);
    ALW_EXPECT_EQ(is_freed, true);
}

ALW_TEST(socket_construction)
{
    MockNetworkApi api;
    AddrInfoList<MockNetworkApi> spec{api, nullptr, nullptr, nullptr};

    Socket<MockNetworkApi> happy{api, spec};
    ALW_EXPECT_EQ(happy.fd(), api.ret_code);

    std::string error{};
    try {
        api.ret_code = MockNetworkApi::ERROR;
        Socket<MockNetworkApi> sad{api, spec};
    }
    catch (std::runtime_error const & e) {
        error = e.what();
    }
    ALW_EXPECT_EQ(error, "socket constructor: " + api.error());
}

ALW_TEST(socket_resource_cleanup)
{
    MockNetworkApi api;
    AddrInfoList<MockNetworkApi> spec{api, nullptr, nullptr, nullptr};

    bool is_closed = false;
    MockNetworkApi::set_is_closed(&is_closed);
    {
        Socket<MockNetworkApi> socket{api, spec};
        ALW_EXPECT_EQ(is_closed, false);
    }
    ALW_EXPECT_EQ(is_closed, true);
    MockNetworkApi::set_is_closed(nullptr);
}

ALW_TEST(socket_move)
{
    MockNetworkApi api;
    AddrInfoList<MockNetworkApi> spec{api, nullptr, nullptr, nullptr};
    Socket<MockNetworkApi> socket1{api, spec};
    Socket<MockNetworkApi> socket2{api, spec};

    bool is_closed = false;
    MockNetworkApi::set_is_closed(&is_closed);

    Socket<MockNetworkApi> newSocket = std::move(socket1);
    socket1.~Socket<MockNetworkApi>();  // NULL_FD
    ALW_EXPECT_EQ(is_closed, false);

    newSocket = std::move(socket2);
    socket2.~Socket<MockNetworkApi>();  // Has socket1's fd
    ALW_EXPECT_EQ(is_closed, true);
    is_closed = false;

    MockNetworkApi::set_is_closed(nullptr);
}

ALW_TEST(socket_comparison)
{
    MockNetworkApi api;
    AddrInfoList<MockNetworkApi> spec{api, nullptr, nullptr, nullptr};

    api.ret_code = 45;
    Socket<MockNetworkApi> x{api, spec};
    ALW_EXPECT_EQ(x.fd(), 45);
    ALW_EXPECT_EQ(x, x);

    api.ret_code = 99;
    Socket<MockNetworkApi> y{api, spec};
    ALW_EXPECT_EQ(y.fd(), 99);
    ALW_EXPECT_EQ(y, y);

    ALW_EXPECT_EQ(x != y, true);
    ALW_EXPECT_EQ(x  < y, true);
    ALW_EXPECT_EQ(x <= y, true);
    ALW_EXPECT_EQ(y  > x, true);
    ALW_EXPECT_EQ(y >= x, true);
}

ALW_TEST(socket_bind)
{
    MockNetworkApi api;
    AddrInfoList<MockNetworkApi> spec{api, nullptr, nullptr, nullptr};

    Socket<MockNetworkApi> happy{api, spec};
    happy.bind(*spec.current());

    std::string error{};
    try {
        Socket<MockNetworkApi> sad{api, spec};
        api.ret_code = MockNetworkApi::ERROR;
        sad.bind(*spec.current());
    }
    catch (std::runtime_error const & e) {
        error = e.what();
    }
    ALW_EXPECT_EQ(error, err_msg("bind", api.error()));
}

ALW_TEST(socket_connect)
{
    MockNetworkApi api;
    AddrInfoList<MockNetworkApi> spec{api, nullptr, nullptr, nullptr};

    Socket<MockNetworkApi> happy{api, spec};
    happy.connect(*spec.current());

    std::string error{};
    try {
        Socket<MockNetworkApi> sad{api, spec};
        api.ret_code = MockNetworkApi::ERROR;
        sad.connect(*spec.current());
    }
    catch (std::runtime_error const & e) {
        error = e.what();
    }
    ALW_EXPECT_EQ(error, err_msg("connect", api.error()));
}

ALW_TEST(socket_listen)
{
    MockNetworkApi api;
    AddrInfoList<MockNetworkApi> spec{api, nullptr, nullptr, nullptr};

    Socket<MockNetworkApi> happy{api, spec};
    happy.listen(0);

    std::string error{};
    try {
        Socket<MockNetworkApi> sad{api, spec};
        api.ret_code = MockNetworkApi::ERROR;
        sad.listen(0);
    }
    catch (std::runtime_error const & e) {
        error = e.what();
    }
    ALW_EXPECT_EQ(error, err_msg("listen", api.error()));
}

ALW_TEST(socket_accept)
{
    MockNetworkApi api;
    AddrInfoList<MockNetworkApi> spec{api, nullptr, nullptr, nullptr};
    Socket<MockNetworkApi> sock{api, spec};

    MockNetworkApi::SockAddr addr = {0xB00, {0,69,2}};
    api.ai.ai_addr = &addr;

    SockInfo<MockNetworkApi> happy_info{};
    Socket<MockNetworkApi> happy = sock.accept(happy_info);

    ALW_EXPECT_EQ(happy_info.addr.sa_family, addr.sa_family);
    ALW_EXPECT_EQ(happy_info.addr.sa_data[1], addr.sa_data[1]);

    std::string error{};
    try {
        SockInfo<MockNetworkApi> sad_info{};
        api.ret_code = MockNetworkApi::ERROR;
        sock.accept(sad_info);
    }
    catch (std::runtime_error const & e) {
        error = e.what();
    }
    ALW_EXPECT_EQ(error, err_msg("accept", api.error()));
}

ALW_TEST(socket_set_option)
{
    MockNetworkApi api;
    AddrInfoList<MockNetworkApi> spec{api, nullptr, nullptr, nullptr};
    Socket<MockNetworkApi> happy{api, spec};
    happy.set_socket_option(1, 2, 3);

    std::string error{};
    try {
        Socket<MockNetworkApi> sad{api, spec};
        api.ret_code = MockNetworkApi::ERROR;
        sad.set_socket_option(1, 2, 3);
    }
    catch (std::runtime_error const & e) {
        error = e.what();
    }
    ALW_EXPECT_EQ(error, err_msg("set_socket_option", api.error()));
}

ALW_TEST(socket_set_file_option)
{
    MockNetworkApi api;
    AddrInfoList<MockNetworkApi> spec{api, nullptr, nullptr, nullptr};
    Socket<MockNetworkApi> happy{api, spec};
    happy.set_file_option(1, 2);

    std::string error{};
    try {
        Socket<MockNetworkApi> sad{api, spec};
        api.ret_code = MockNetworkApi::ERROR;
        sad.set_file_option(1, 2);
    }
    catch (std::runtime_error const & e) {
        error = e.what();
    }
    ALW_EXPECT_EQ(error, err_msg("set_file_option", api.error()));
}

}  // namespace alewa::net::test
