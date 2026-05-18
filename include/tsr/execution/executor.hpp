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

    // =========================================== Validations ===========================================
    template <typename T, typename Context>
    concept HasStaticRunWithContext = requires(Context& ctx) { T::Run(ctx); };
    template <typename T>
    concept HasStaticRunNoContext = requires { T::Run(); };

    template <typename>
    inline constexpr bool always_false_executor_v = false;

    template <typename T>
    struct Executor
    {
        template <MissingExecutorPolicy MissingPolicy = MissingExecutorPolicy::Assert, typename Context>
        static void Run(Context& context)
        {
            if constexpr (HasStaticRunWithContext<T, Context>)
            {
                T::Run(context);
            }
            else if constexpr (HasStaticRunNoContext<T>)
            {
                T::Run();
            }
            else
            {
                if constexpr (MissingPolicy == MissingExecutorPolicy::Assert)
                {
                    static_assert(always_false_executor_v<T>,
                                  "Executor<T> specialization is missing, and T has no static Run");
                }
                else if constexpr (MissingPolicy == MissingExecutorPolicy::Skip)
                {
                    // no-op
                }
                else if constexpr (MissingPolicy == MissingExecutorPolicy::Warn)
                {
                    std::cerr << "[tsr] warning: missing executor; skipped\n";
                }
            }
        }
    };

    // ============================================ Execute Order =============================================
    template <typename NodePackT>
    struct ExecuteOrder;

    template <typename... NodeTs>
    struct ExecuteOrder<NodePack<Node<NodeTs>...>>
    {
        template <typename Context>
        static void Run(Context& context)
        {
            (Executor<NodeTs>::template Run<MissingExecutorPolicy::Assert>(context), ...);
        }
    };

    // ============================================ Execute Plan =============================================
    template <typename PlanT, MissingExecutorPolicy MissingPolicy = MissingExecutorPolicy::Assert>
    struct ExecutePlan;
    template <typename... NodeTs, MissingExecutorPolicy MissingPolicy>
    struct ExecutePlan<SequentialPlan<NodePack<Node<NodeTs>...>>, MissingPolicy>
    {
        template <typename Context>
        static void Run(Context& context)
        {
            (Executor<NodeTs>::template Run<MissingPolicy>(context), ...);
        }
    };

    template <typename MetaPlanT, typename SubPlanPackT, MissingExecutorPolicy MissingPolicy>
    struct ExecuteHierarchicalPlan;
    template <typename... GraphTs, typename SubPlanPackT, MissingExecutorPolicy MissingPolicy>
    struct ExecuteHierarchicalPlan<SequentialPlan<NodePack<Node<GraphTs>...>>, SubPlanPackT, MissingPolicy>
    {
        template <typename Context>
        static void Run(Context& context)
        {
            (ExecutePlan<typename FindSubPlan<GraphTs, SubPlanPackT>::type::plan_type, MissingPolicy>::Run(context), ...);
        }
    };
    template <typename MetaPlanT, typename SubPlanPackT, MissingExecutorPolicy MissingPolicy>
    struct ExecutePlan<HierarchicalPlan<MetaPlanT, SubPlanPackT>, MissingPolicy>
    {
        template <typename Context>
        static void Run(Context& context)
        {
            ExecuteHierarchicalPlan<MetaPlanT, SubPlanPackT, MissingPolicy>::Run(context);
        }
    };
}; // namespace tsr