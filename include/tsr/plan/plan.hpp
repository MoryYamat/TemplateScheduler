#pragma once

#include "tsr/compiler/topological_sort.hpp"

namespace tsr
{
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

    template <typename TargetGraphT, typename FirstSubPlan, typename... Rest>
    struct FindSubPlan<TargetGraphT, SubPlanPack<FirstSubPlan, Rest...>>
        : FindSubPlanImpl<std::is_same_v<TargetGraphT, typename FirstSubPlan::graph_type>, TargetGraphT, FirstSubPlan,
                          Rest...>
    {
    };
    // find and match the metaplan
    // template <typename GraphT, typename SubPlanPackT>
    // struct FindSubPlan;
    // template <typename GraphT, typename FirstSubPlan, typename... Rest>
    // struct FindSubPlan<GraphT, SubPlanPack<FirstSubPlan, Rest...>>
    // {
    //     using type = std::conditional_t<std::is_same_v<GraphT, typename FirstSubPlan::graph_type>, FirstSubPlan,
    //                                     typename FindSubPlan<GraphT, SubPlanPack<Rest...>>::type>;
    // };

} // namespace tsr