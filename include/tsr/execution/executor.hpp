/*
An execution platform for actually using the compiler's output.

**The important point is that execution doesn't need to know about graphs, arcs, or relations.**
**It's sufficient if it only accepts a "something that looks like an executable plan" like NodePack<...> and works accordingly.**
*/

#pragma once
#include <iostream>
#include <array>
#include <future>
#include <cstddef>
// #include "tsr/graph/graph_dsl.hpp"
// #include "tsr/compiler/topological_sort.hpp" // Delete the hierarchical DSL IR once it has been created.

#include "tsr/plan/plan.hpp"
#include "tsr/adapter/adapter.hpp"

namespace tsr
{
    // =========================================== Policy ===========================================
    enum class MissingExecutorPolicy
    {
        Assert,
        Skip,
        Warn
    };

    struct DefaultExecutionConfig
    {
        static constexpr MissingExecutorPolicy missing_executor_policy = MissingExecutorPolicy::Assert;
        static constexpr bool emit_barrier_log = false;
    };
    struct WarnExecutionConfig
    {
        static constexpr MissingExecutorPolicy missing_executor_policy = MissingExecutorPolicy::Warn;
        static constexpr bool emit_barrier_log = true;
    };
    struct SkipExecutionConfig
    {
        static constexpr MissingExecutorPolicy missing_executor_policy = MissingExecutorPolicy::Skip;
        static constexpr bool emit_barrier_log = false;
    };
    struct DebugExecutionConfig
    {
        static constexpr MissingExecutorPolicy missing_executor_policy = MissingExecutorPolicy::Warn;
        static constexpr bool emit_barrier_log = true;
    };



    // =========================================== Validations ===========================================
    template <typename T, typename Context>
    concept HasStaticRunWithContext = requires(Context& ctx) { T::Run(ctx); };
    template <typename T>
    concept HasStaticRunNoContext = requires { T::Run(); };

    template <typename>
    inline constexpr bool always_false_executor_v = false;

    // user adaptor
    template <typename T>
    struct Executor
    {
    };

    template <typename T>
    struct ExecutorDispatch
    {
        template <typename ConfigT = DefaultExecutionConfig, typename Context>
        static void Run(Context& context)
        {
            if constexpr (requires { Executor<T>::template Run<ConfigT>(context); })
            {
                Executor<T>::template Run<ConfigT>(context);
            }
            else if constexpr (requires { Executor<T>::Run(context); })
            {
                Executor<T>::Run(context);
            }
            else if constexpr (requires { Executor<T>::template Run<ConfigT>(); })
            {
                Executor<T>::template Run<ConfigT>();
            }
            else if constexpr (requires { Executor<T>::Run(); })
            {
                Executor<T>::Run();
            }
            else if constexpr (HasStaticRunWithContext<T, Context>)
            {
                T::Run(context);
            }
            else if constexpr (HasStaticRunNoContext<T>)
            {
                T::Run();
            }
            else
            {
                constexpr auto policy = ConfigT::missing_executor_policy;
                if constexpr (policy == MissingExecutorPolicy::Assert)
                {
                    static_assert(always_false_executor_v<T>,
                                  "Executor<T> specialization is missing, and T has no static Run");
                }
                else if constexpr (policy == MissingExecutorPolicy::Skip)
                {
                    // no-op
                }
                else if constexpr (policy == MissingExecutorPolicy::Warn)
                {
                    std::cerr << "[tsr] warning: missing executor; skipped\n";
                }
            }
        }
    };
    template <typename T>
    struct ExecutorDispatch<Node<T>>
    {
        template<typename ConfigT = DefaultExecutionConfig, typename Context>
        static void Run(Context& context)
        {
            ExecutorDispatch<T>::template Run<ConfigT>(context);
        }
    };

    template <typename ConfigT>
    struct BarrierDispatch
    {
        template <typename Context>
        static void Run(Context&)
        {
            if constexpr (ConfigT::emit_barrier_log)
            {
                std::cerr << "[tsr] barrier\n";
            }
        }
    };

    // ============================================ Execute Order =============================================
    template <typename NodePackT, typename ConfigT = DefaultExecutionConfig>
    struct ExecuteOrder;

    template <typename... NodeTs, typename ConfigT>
    struct ExecuteOrder<NodePack<Node<NodeTs>...>, ConfigT>
    {
        template <typename Context>
        static void Run(Context& context)
        {
            (ExecutorDispatch<NodeTs>::template Run<ConfigT>(context), ...);
        }
    };

    // ============================================ Execute Plan =============================================
    template <typename PlanT, typename ConfigT = DefaultExecutionConfig>
    struct ExecutePlan;
    template <typename... NodeTs, typename ConfigT>
    struct ExecutePlan<SequentialPlan<NodePack<Node<NodeTs>...>>, ConfigT>
    {
        template <typename Context>
        static void Run(Context& context)
        {
            (ExecutorDispatch<NodeTs>::template Run<ConfigT>(context), ...);
        }
    };

    template <typename MetaPlanT, typename SubPlanPackT, typename ConfigT = DefaultExecutionConfig>
    struct ExecuteHierarchicalPlan;
    template <typename... GraphTs, typename SubPlanPackT, typename ConfigT>
    struct ExecuteHierarchicalPlan<SequentialPlan<NodePack<Node<GraphTs>...>>, SubPlanPackT, ConfigT>
    {
        template <typename Context>
        static void Run(Context& context)
        {
            (ExecutePlan<typename FindSubPlan<GraphTs, SubPlanPackT>::type::plan_type, ConfigT>::Run(context), ...);
        }
    };
    template <typename MetaPlanT, typename SubPlanPackT, typename ConfigT>
    struct ExecutePlan<HierarchicalPlan<MetaPlanT, SubPlanPackT>, ConfigT>
    {
        template <typename Context>
        static void Run(Context& context)
        {
            ExecuteHierarchicalPlan<MetaPlanT, SubPlanPackT, ConfigT>::Run(context);
        }
    };
    template <typename... NodePackTs, typename ConfigT>
    struct ExecutePlan<LayeredPlan<LayerPack<NodePackTs...>>, ConfigT>
    {
        template <typename Context>
        static void Run(Context& context)
        {
            ((ExecuteOrder<NodePackTs, ConfigT>::Run(context), BarrierDispatch<ConfigT>::Run(context)), ...);
        }
    };

    // ==================================== Execute SafeLayeredPlan ====================================
    template <typename... NodePackTs, typename ConfigT>
    struct ExecutePlan<SafeLayeredPlan<LayerPack<NodePackTs...>>, ConfigT>
    {
        template <typename Context>
        static void Run(Context& context)
        {
            ((ExecuteOrder<NodePackTs, ConfigT>::Run(context), BarrierDispatch<ConfigT>::Run(context)), ...);
        }
    };
    template <typename NodePackT, typename ConfigT>
    struct ExecuteLayerAsync;
    template <typename... Ts, typename ConfigT>
    struct ExecuteLayerAsync<NodePack<Node<Ts>...>, ConfigT>
    {
        template <typename Context>
        static void Run(Context& context)
        {
            auto futures = std::array{
                    std::async(std::launch::async, [&context] { 
                        ExecutorDispatch<Ts>::template Run<ConfigT>(context); 
                    })...
                };

            for(auto& f : futures)
            {
                f.get();
            }
            BarrierDispatch<ConfigT>::Run(context);
        }
    };
    template<typename ConfigT>
    struct ExecuteLayerAsync<NodePack<>, ConfigT>
    {
        template<typename Context>
        static void Run(Context& context)
        {
            BarrierDispatch<ConfigT>::Run(context);
        }
    };

    template <typename LayerPackT, typename ConfigT>
    struct ExecuteLayerPackAsync;
    template<typename... NodePackTs, typename ConfigT>
    struct ExecuteLayerPackAsync<LayerPack<NodePackTs...>, ConfigT>
    {
        template<typename Context>
        static void Run(Context& context)
        {
            (ExecuteLayerAsync<NodePackTs, ConfigT>::Run(context),...);
        }
    };

    template <typename LayerPackT, typename ConfigT>
    struct ExecutePlanAsync;
    template <typename LayerPackT, typename ConfigT>
    struct ExecutePlanAsync<SafeLayeredPlan<LayerPackT>, ConfigT>
    {
        template <typename Context>
        static void Run(Context& context)
        {
            ExecuteLayerPackAsync<LayerPackT, ConfigT>::Run(context);
        }
    };

    // ================= ExecutePlanWithPool =================
    template<typename NodePackT, typename ConfigT>
    struct ExecuteLayerWithPool;
    template<typename... Ts, typename ConfigT>
    struct ExecuteLayerWithPool<NodePack<Node<Ts>...>, ConfigT>
    {
        template<typename PoolT, typename Context>
        static void Run(PoolT& pool, Context& context)
        {
            auto futures = std::array{
                    PoolAdapter<PoolT>::Submit(pool, [&context] {
                        ExecutorDispatch<Ts>::template Run<ConfigT>(context);
                })...
            };

            for(auto& f : futures)
            {
                f.get();
            }

            BarrierDispatch<ConfigT>::Run(context);
        }
    };

    template<typename LayerPackT, typename ConfigT>
    struct ExecuteLayerPackWithPool;
    template<typename... NodePackTs, typename ConfigT>
    struct ExecuteLayerPackWithPool<LayerPack<NodePackTs...>, ConfigT>
    {
        template<typename PoolT, typename Context>
        static void Run(PoolT& pool, Context& context)
        {
            (ExecuteLayerWithPool<NodePackTs, ConfigT>::Run(pool, context),...);
        }
    };

    template<typename PlanT, typename ConfigT = DefaultExecutionConfig>
    struct ExecutePlanWithPool;
    template<typename LayerPackT, typename ConfigT>
    struct ExecutePlanWithPool<SafeLayeredPlan<LayerPackT>,  ConfigT>
    {
        template<typename PoolT, typename Context>
        static void Run(PoolT& pool, Context& context)
        {
            ExecuteLayerPackWithPool<LayerPackT, ConfigT>::Run(pool, context);
        }
    };
    
}; // namespace tsr