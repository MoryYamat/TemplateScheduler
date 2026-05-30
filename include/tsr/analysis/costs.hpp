#pragma once
#include <cstddef>

namespace tsr
{
    template<typename T>
    struct DeclaredCost
    {
        static constexpr std::size_t value = 1;
    };
}