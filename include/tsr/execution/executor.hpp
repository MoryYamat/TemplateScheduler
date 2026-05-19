/*
An execution platform for actually using the compiler's output.

**The important point is that execution doesn't need to know about graphs, arcs, or relations.**
**It's sufficient if it only accepts a "something that looks like an executable plan" like NodePack<...> and works accordingly.**
*/

#pragma once
#include <iostream>
// #include "tsr/graph/graph_dsl.hpp"
// #include "tsr/compiler/topological_sort.hpp" // Delete the hierarchical DSL IR once it has been created.

#include "tsr/plan/plan.hpp"

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
    };
    struct WarnExecutionConfig
    {
        static constexpr MissingExecutorPolicy missing_executor_policy = MissingExecutorPolicy::Warn;
    };
    struct SkipExecutionConfig
    {
        static constexpr MissingExecutorPolicy missing_executor_policy = MissingExecutorPolicy::Skip;
    };

    // =========================================== Validations ===========================================
    template <typename T, typename Context>
    concept HasStaticRunWithContext = requires(Context& ctx) { T::Run(ctx); };
    template <typename T>
    concept HasStaticRunNoContext = requires { T::Run(); };

    template <typename>
    inline constexpr bool always_false_executor_v = false;

    // user adaptor
    template<typename T>
    struct Executor{};

    template <typename T>
    struct ExecutorDispatch
    {
        template <typename ConfigT = DefaultExecutionConfig, typename Context>
        static void Run(Context& context)
        {
            if constexpr (requires {Executor<T>::template Run<ConfigT>(context);})
            {
                Executor<T>::template Run<ConfigT>(context);
            }
            else if constexpr (requires { Executor<T>::Run(context);})
            {
                Executor<T>::Run(context);
            }
            else if constexpr (requires { Executor<T>::template Run<ConfigT>();})
            {
                Executor<T>::template Run<ConfigT>();
            }
            else if constexpr (requires { Executor<T>::Run();})
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
}; // namespace tsr