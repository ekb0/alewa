add_library(alewa_compiler_flags INTERFACE)

target_compile_options(alewa_compiler_flags
    INTERFACE
        -Wall
        -Wextra
        -Wconversion
        -pedantic-errors
)

add_executable(alewa
    alewa.cpp
    alewa/net/netapi.cpp
    alewa/net/network.cpp
)

target_link_libraries(alewa
    PUBLIC
        alewa_compiler_flags
)

add_executable(alewa_test
    alewa/test/test.cpp
)

target_link_libraries(alewa_test
    PRIVATE
        alewa_compiler_flags
)

add_test(
    NAME alewa_test_runner
    COMMAND alewa_test
)
