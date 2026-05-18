/*
An execution platform for actually using the compiler's output.

**The important point is that execution doesn't need to know about graphs, arcs, or relations.**
**It's sufficient if it only accepts a "something that looks like an executable plan" like NodePack<...> and works accordingly.**
*/

#pragma once

#include "tsr/graph/graph_dsl.hpp"
#include "tsr/compiler/topological_sort.hpp" // Delete the hierarchical DSL IR once it has been created.

namespace tsr
{
    template <typename>
    inline constexpr bool always_false_executor_v = false;

    template <typename T>
    struct Executor
    {
        static_assert(always_false_executor_v<T>, "Executor<T> specialization is missing");
    };
    
    // ====================== Execute Order =======================
    template <typename NodePackT>
    struct ExecuteOrder;
    template <typename... NodeTs>
    struct ExecuteOrder<NodePack<Node<NodeTs>...>>
    {
        template <typename Context>
        static void Run(Context& context)
        {
            (Executor<NodeTs>::Run(context), ...);
        }
    };

    // ====================== Execute Plan =======================
    template <typename PlanT>
    struct ExecutePlan;
    template <typename... NodeTs>
    struct ExecutePlan<SequentialPlan<NodePack<Node<NodeTs>...>>>
    {
        template <typename Context>
        static void Run(Context& context)
        {
            (Executor<NodeTs>::Run(context), ...);
        }
    };

    // ====================== Execute HierarchicalPlan =======================
    // find and match the metaplan
    template <typename>
    inline constexpr bool always_false_v = false;

    template <typename GraphT, typename SubPlanPackT>
    struct FindSubPlan;
    template <typename GraphT>
    struct FindSubPlan<GraphT, SubPlanPack<>>
    {
        static_assert(always_false_v<GraphT>, "SubPlan for GraphT was not found in SubPlanPack");
    };

    template <bool Match, typename GraphT, typename FirstSubPlan, typename... Rest>
    struct FindSubPlanImpl;
    template <typename GraphT, typename FirstSubPlan, typename... Rest>
    struct FindSubPlanImpl<true, GraphT, FirstSubPlan, Rest...>
    {
        using type = FirstSubPlan;
    };
    template <typename GraphT, typename FirstSubPlan, typename... Rest>
    struct FindSubPlanImpl<false, GraphT, FirstSubPlan, Rest...>
    {
        using type = typename FindSubPlan<GraphT, SubPlanPack<Rest...>>::type;
    };

    template <typename GraphT, typename FirstSubPlan, typename... Rest>
    struct FindSubPlan<GraphT, SubPlanPack<FirstSubPlan, Rest...>>
        : FindSubPlanImpl<std::is_same_v<GraphT, typename FirstSubPlan::graph_type>, GraphT, FirstSubPlan, Rest...>
    {
    };

    template <typename MetaPlanT, typename SubPlanPackT>
    struct ExecuteHierarchicalPlan;
    template <typename... GraphTs, typename SubPlanPackT>
    struct ExecuteHierarchicalPlan<SequentialPlan<NodePack<Node<GraphTs>...>>, SubPlanPackT>
    {
        template <typename Context>
        static void Run(Context& context)
        {
            (ExecutePlan<typename FindSubPlan<GraphTs, SubPlanPackT>::type::plan_type>::Run(context), ...);
        }
    };

    // ====================== Execute Plan =======================
    template <typename MetaPlanT, typename SubPlanPackT>
    struct ExecutePlan<HierarchicalPlan<MetaPlanT, SubPlanPackT>>
    {
        template <typename Context>
        static void Run(Context& context)
        {
            ExecuteHierarchicalPlan<MetaPlanT, SubPlanPackT>::Run(context);
        }
    };
}; // namespace tsr