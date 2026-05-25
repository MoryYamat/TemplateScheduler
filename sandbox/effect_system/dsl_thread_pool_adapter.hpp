#pragma once

#include "user_thread_pool.hpp"
#include "tsr/adapter/adapter.hpp"

namespace tsr
{
    // test for adapter
    template<>
    struct PoolAdapter<es::MyThreadPool>
    {
        template<typename F>
        static auto Submit(es::MyThreadPool& pool, F&& f)
        {
            return pool.enqueue(std::forward<F>(f));
        }
    };
}