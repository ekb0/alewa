#pragma once

#include <concepts>
#include <string>

namespace alewa::sys {

template <typename T>
concept ErrorDescription = requires(T t)
{
    /* constants */
    { T::ERROR } -> std::same_as<int const &>;
    { T::SUCCESS } -> std::same_as<int const &>;

    /* methods */
    { t.error() } -> std::same_as<std::string>;
};

struct SystemErrorDescription
{
    static int const ERROR = -1;
    static int const SUCCESS = 0;

    [[nodiscard]] std::string error() const;
};

}  // namespace alewa::sys
