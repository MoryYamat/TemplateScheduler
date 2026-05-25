/*
Something like a reverse projection
*/
#pragma once

#include <utility>

namespace tsr
{
    template <typename PoolT>
    struct PoolAdapter
    {
        template <typename F>
        static auto Submit(PoolT& pool, F&& f)
        {
            return pool.Submit(std::forward<F>(f));
        }
    };
} // namespace tsr