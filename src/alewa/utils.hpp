#pragma once

#define ALW_DELEGATE(func, delegate)                                           \
    decltype(&(delegate)) const func = (delegate)
