add_library(alewa_compiler_flags INTERFACE)
add_library(alewa_linker_flags INTERFACE)

target_compile_options(alewa_compiler_flags
    INTERFACE
        -O3
        -Wall
        -Werror
        -Wextra
        -Wconversion
        -pedantic-errors
        # -fsanitize=address
        -D_GLIBCXX_ASSERTIONS
    )

target_link_options(alewa_linker_flags
    INTERFACE
        -fsanitize=address
)

add_executable(alewa
    alewa.cpp
    alewa/server.cpp
    alewa/io/ioapi.cpp
    alewa/io/ioapi_sys.cpp
    alewa/io/socket.cpp
)

target_link_libraries(alewa
    PRIVATE
        alewa_compiler_flags
        # alewa_linker_flags
)

target_include_directories(alewa
    PRIVATE
        alewa/
)

add_executable(alewa_test
    alewa.test.cpp
    alewa/test/test_utils.cpp
    alewa/io/sockapi_mock.cpp
)

target_link_libraries(alewa_test
    PRIVATE
        alewa_compiler_flags
        # alewa_linker_flags
)

target_include_directories(alewa_test
    PRIVATE
        alewa/
)

add_test(
    NAME alewa_test_runner
    COMMAND alewa_test
)
