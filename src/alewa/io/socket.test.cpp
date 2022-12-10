#include "test/test_utils.hpp"

#include "socket.hpp"
#include "sockapi_mock.hpp"

namespace alewa::io::test {

auto err_msg(std::string const & func, std::string const & msg)
{
    return func + " on socket 0: " + msg;
}

ALW_TEST(addrinfolist_construction)
{
    MockSocketApi api;
    AddrInfoList<MockSocketApi> happy{api, nullptr, nullptr, nullptr};
    ALW_EXPECT_EQ(happy.current(), &api.ai);

    std::string error{};
    try {
        api.ret_code = MockSocketApi::ERROR;
        AddrInfoList<MockSocketApi> sad{api, nullptr, nullptr, nullptr};
    }
    catch (std::runtime_error const & e) {
        error = e.what();
    }
    std::string m = std::string{"getaddrinfo: "};
    ALW_EXPECT_EQ(error, m + api.gai_strerror(api.ret_code));
}

ALW_TEST(addrinfolist_resource_cleanup)
{
    MockSocketApi api;
    bool is_freed = false;
    MockSocketApi::set_is_freed(&is_freed);
    {
        AddrInfoList<MockSocketApi> spec{api, nullptr, nullptr, nullptr};
        ALW_EXPECT_EQ(is_freed, false);
    }
    MockSocketApi::set_is_freed(nullptr);
    ALW_EXPECT_EQ(is_freed, true);
}

ALW_TEST(socket_construction)
{
    MockSocketApi api;
    AddrInfoList<MockSocketApi> spec{api, nullptr, nullptr, nullptr};

    Socket<MockSocketApi> happy{api, spec};
    ALW_EXPECT_EQ(happy.fd(), api.ret_code);

    std::string error{};
    try {
        api.ret_code = MockSocketApi::ERROR;
        Socket<MockSocketApi> sad{api, spec};
    }
    catch (std::runtime_error const & e) {
        error = e.what();
    }
    ALW_EXPECT_EQ(error, "socket constructor: " + api.error());
}

ALW_TEST(socket_resource_cleanup)
{
    MockSocketApi api;
    AddrInfoList<MockSocketApi> spec{api, nullptr, nullptr, nullptr};

    bool is_closed = false;
    MockSocketApi::set_is_closed(&is_closed);
    {
        Socket<MockSocketApi> socket{api, spec};
        ALW_EXPECT_EQ(is_closed, false);
    }
    ALW_EXPECT_EQ(is_closed, true);
    MockSocketApi::set_is_closed(nullptr);
}

ALW_TEST(socket_move)
{
    MockSocketApi api;
    AddrInfoList<MockSocketApi> spec{api, nullptr, nullptr, nullptr};
    std::unique_ptr<Socket<MockSocketApi>> movedTo;

    bool is_closed = false;
    MockSocketApi::set_is_closed(&is_closed);

    {
        Socket<MockSocketApi> socket1{api, spec};
        movedTo = std::make_unique<Socket<MockSocketApi>>(std::move(socket1));
        /* socket1 now has NULL_FD, nothing to close when destructor called */
    }
    ALW_EXPECT_EQ(is_closed, false);

    {
        Socket<MockSocketApi> socket2{api, spec};
        *movedTo = std::move(socket2);
        /* socket2 now has socket1's FD, will close it when destructor called */
    }
    ALW_EXPECT_EQ(is_closed, true);

    is_closed = false;
    MockSocketApi::set_is_closed(nullptr);
}

ALW_TEST(socket_comparison)
{
    MockSocketApi api;
    AddrInfoList<MockSocketApi> spec{api, nullptr, nullptr, nullptr};

    api.ret_code = 45;
    Socket<MockSocketApi> x{api, spec};
    ALW_EXPECT_EQ(x.fd(), 45);
    ALW_EXPECT_EQ(x, x);

    api.ret_code = 99;
    Socket<MockSocketApi> y{api, spec};
    ALW_EXPECT_EQ(y.fd(), 99);
    ALW_EXPECT_EQ(y, y);

    ALW_EXPECT_EQ(x != y, true);
    ALW_EXPECT_EQ(x <  y, true);
    ALW_EXPECT_EQ(x <= y, true);
    ALW_EXPECT_EQ(y >  x, true);
    ALW_EXPECT_EQ(y >= x, true);
}

ALW_TEST(socket_bind)
{
    MockSocketApi api;
    AddrInfoList<MockSocketApi> spec{api, nullptr, nullptr, nullptr};

    Socket<MockSocketApi> happy{api, spec};
    happy.bind(*spec.current());

    std::string error{};
    try {
        Socket<MockSocketApi> sad{api, spec};
        api.ret_code = MockSocketApi::ERROR;
        sad.bind(*spec.current());
    }
    catch (std::runtime_error const & e) {
        error = e.what();
    }
    ALW_EXPECT_EQ(error, err_msg("bind", api.error()));
}

ALW_TEST(socket_connect)
{
    MockSocketApi api;
    AddrInfoList<MockSocketApi> spec{api, nullptr, nullptr, nullptr};

    Socket<MockSocketApi> happy{api, spec};
    happy.connect(*spec.current());

    std::string error{};
    try {
        Socket<MockSocketApi> sad{api, spec};
        api.ret_code = MockSocketApi::ERROR;
        sad.connect(*spec.current());
    }
    catch (std::runtime_error const & e) {
        error = e.what();
    }
    ALW_EXPECT_EQ(error, err_msg("connect", api.error()));
}

ALW_TEST(socket_listen)
{
    MockSocketApi api;
    AddrInfoList<MockSocketApi> spec{api, nullptr, nullptr, nullptr};

    Socket<MockSocketApi> happy{api, spec};
    happy.listen(0);

    std::string error{};
    try {
        Socket<MockSocketApi> sad{api, spec};
        api.ret_code = MockSocketApi::ERROR;
        sad.listen(0);
    }
    catch (std::runtime_error const & e) {
        error = e.what();
    }
    ALW_EXPECT_EQ(error, err_msg("listen", api.error()));
}

ALW_TEST(socket_accept)
{
    MockSocketApi api;
    AddrInfoList<MockSocketApi> spec{api, nullptr, nullptr, nullptr};
    Socket<MockSocketApi> sock{api, spec};

    MockSocketApi::SockAddr addr = {0xB00, {0, 69, 2}};
    api.ai.ai_addr = &addr;

    SockInfo<MockSocketApi> happy_info{};
    Socket<MockSocketApi> happy = sock.accept(happy_info);

    ALW_EXPECT_EQ(happy_info.addr.sa_family, addr.sa_family);
    ALW_EXPECT_EQ(happy_info.addr.sa_data[1], addr.sa_data[1]);

    std::string error{};
    try {
        SockInfo<MockSocketApi> sad_info{};
        api.ret_code = MockSocketApi::ERROR;
        sock.accept(sad_info);
    }
    catch (std::runtime_error const & e) {
        error = e.what();
    }
    ALW_EXPECT_EQ(error, err_msg("accept", api.error()));
}

ALW_TEST(socket_set_option)
{
    MockSocketApi api;
    AddrInfoList<MockSocketApi> spec{api, nullptr, nullptr, nullptr};
    Socket<MockSocketApi> happy{api, spec};
    happy.set_socket_option(1, 2, 3);

    std::string error{};
    try {
        Socket<MockSocketApi> sad{api, spec};
        api.ret_code = MockSocketApi::ERROR;
        sad.set_socket_option(1, 2, 3);
    }
    catch (std::runtime_error const & e) {
        error = e.what();
    }
    ALW_EXPECT_EQ(error, err_msg("set_socket_option", api.error()));
}

ALW_TEST(socket_set_file_option)
{
    MockSocketApi api;
    AddrInfoList<MockSocketApi> spec{api, nullptr, nullptr, nullptr};
    Socket<MockSocketApi> happy{api, spec};
    happy.set_file_option(1, 2);

    std::string error{};
    try {
        Socket<MockSocketApi> sad{api, spec};
        api.ret_code = MockSocketApi::ERROR;
        sad.set_file_option(1, 2);
    }
    catch (std::runtime_error const & e) {
        error = e.what();
    }
    ALW_EXPECT_EQ(error, err_msg("set_file_option", api.error()));
}

}  // namespace alewa::io::test
