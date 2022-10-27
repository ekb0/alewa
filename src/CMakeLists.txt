add_library(alewa_compiler_flags INTERFACE)

target_compile_options(alewa_compiler_flags
    INTERFACE
        -v
        -Wall
        -Wextra
        -Wconversion
        -pedantic-errors
)

add_executable(alewa
    alewa.cpp
)

target_link_libraries(alewa
    PUBLIC
        alewa_compiler_flags
)

add_executable(alewa_test
    ${CMAKE_CURRENT_SOURCE_DIR}/alewa/test/test.cpp
)

target_link_libraries(alewa_test
    PRIVATE
        alewa_compiler_flags
)

add_test(
    NAME alewa_test_runner
    COMMAND alewa_test
)
