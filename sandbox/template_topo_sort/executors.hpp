#pragma once

#include <iostream>

#include "tsr/executor/executor.hpp"
#include "components.hpp"

namespace tsr
{
    // All ctx objects passed in a single ExecuteOrder<Order>::Run(ctx) call are of the same type and instance.
    // RuntimeContext ctx{};
    // ExecuteOrder<Order>::Run(ctx);
    // struct RuntimeContext
    // {
    //     World& world;
    //     float delta_time;
    //     Logger* logger;
    // };
    // - ctx.world
    // - ctx.logger
    // ...
    struct Context
    {
    };

    template <>
    struct Executor<Root>
    {
        template <typename ConfigT = DefaultExecutionConfig, typename Context>
        static void Run(Context& context)
        {
            std::cerr << "Root\n";
        }
    };

    // template <>
    // struct Executor<Physics>
    // {
    //     template <typename Context>
    //     static void Run(Context& ctx)
    //     {
    //         if constexpr (requires { ctx.world; })
    //         {
    //             // runtime execution
    //         }
    //         if constexpr (requires { ctx.logger; })
    //         {
    //             // validation mode
    //         }
    //     }
    // };

    template <>
    struct Executor<CLeft_1>
    {
        template <typename ConfigT, typename Context>
        static void Run(Context& context)
        {
            std::cerr << "CLeft_1\n";
        }
    };

    template <>
    struct Executor<CRight_1>
    {
        template <typename ConfigT, typename Context>
        static void Run(Context& context)
        {
            std::cerr << "CRight_1\n";
        }
    };

    template <>
    struct Executor<CRight_2>
    {
        template <typename ConfigT, typename Context>
        static void Run(Context& context)
        {
            std::cerr << "CRight_2\n";
        }
    };

    template <>
    struct Executor<CCLeft_1>
    {
        template <typename ConfigT, typename Context>
        static void Run(Context& context)
        {
            std::cerr << "CCLeft_1\n";
        }
    };

    template <>
    struct Executor<CCLeft_2>
    {
        template <typename ConfigT, typename Context>
        static void Run(Context& context)
        {
            std::cerr << "CCLeft_2\n";
        }
    };

    template <>
    struct Executor<CCRight_1>
    {
        template <typename ConfigT, typename Context>
        static void Run(Context& context)
        {
            std::cerr << "CCRight_1\n";
        }
    };

    template <>
    struct Executor<CCCLeft_1>
    {
        template <typename ConfigT, typename Context>
        static void Run(Context& context)
        {
            std::cerr << "CCCLeft_1\n";
        }
    };

    template <>
    struct Executor<CCCRight_1>
    {
        template <typename ConfigT, typename Context>
        static void Run(Context& context)
        {
            std::cerr << "CCCRight_1\n";
        }
    };

    template <>
    struct Executor<Pos>
    {
        template <typename ConfigT, typename Context>
        static void Run(Context& context)
        {
            std::cerr << "Pos\n";
        }
    };
    template <>
    struct Executor<Vel>
    {
        template <typename ConfigT, typename Context>
        static void Run(Context& context)
        {
            std::cerr << "Vel\n";
        }
    };
    template <>
    struct Executor<Acc>
    {
        template <typename ConfigT, typename Context>
        static void Run(Context& context)
        {
            std::cerr << "Acc\n";
        }
    };
    template <>
    struct Executor<Dis>
    {
        template <typename ConfigT, typename Context>
        static void Run(Context& context)
        {
            std::cerr << "Dis\n";
        }
    };

    template <>
    struct Executor<Collision>
    {
        template <typename ConfigT, typename Context>
        static void Run(Context& context)
        {
            std::cerr << "Collision\n";
        }
    };
    // template <>
    // struct Executor<CollisionMask>
    // {
    //     template <typename Context>
    //     static void Run(Context& context)
    //     {
    //         std::cerr << "CollisionMask\n";
    //     }
    // };

    template<>
    struct Executor<A>
    {
        template<typename ConfigT = DefaultExecutionConfig, typename Context>
        static void Run(Context& ctx)
        {
            ctx.log.push_back("executor_config");
        }
    };

    template<>
    struct Executor<B>
    {
        template<typename Context>
        static void Run(Context& ctx)
        {
            ctx.log.push_back("executor_plain");
        }
    };

    
    // ============= For testing Layered Plan =============
} // namespace tsr