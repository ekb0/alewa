add_library(alewa_compiler_flags INTERFACE)

target_compile_options(alewa_compiler_flags
    INTERFACE
        -O3
        -Wall
        -Wextra
        -Wconversion
        -pedantic-errors
)

add_executable(alewa
    alewa.cpp
    alewa/net/netapi.cpp
    alewa/net/sysnetapi.cpp
    alewa/net/network.cpp
)

target_link_libraries(alewa
    PRIVATE
        alewa_compiler_flags
)

target_include_directories(alewa
    PRIVATE
        alewa/
)

add_executable(alewa_test
    alewa/test/test.cpp
    alewa/net/mocknetapi.cpp
)

target_link_libraries(alewa_test
    PRIVATE
        alewa_compiler_flags
)

target_include_directories(alewa_test
    PRIVATE
        alewa/
)

add_test(
    NAME alewa_test_runner
    COMMAND alewa_test
)
