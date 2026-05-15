/*
An execution platform for actually using the compiler's output.

**The important point is that execution doesn't need to know about graphs, arcs, or relations.**
**It's sufficient if it only accepts a "something that looks like an executable plan" like NodePack<...> and works accordingly.**
*/

#pragma once

#include "tsr/graph/graph_dsl.hpp"

namespace tsr
{
    template<typename T>
    struct Executor;

    // ====================== Execute Order =======================
    template<typename NodePackT>
    struct ExecuteOrder;
    template<typename... NodeTs>
    struct ExecuteOrder<NodePack<Node<NodeTs>...>>
    {
        template<typename Context>
        static void Run(Context& context)
        {
            (Executor<NodeTs>::Run(context),...);
        }
    };
};