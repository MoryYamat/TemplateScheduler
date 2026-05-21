#pragma once

#include "tsr/compiler/topological_sort.hpp"

namespace tsr
{
    // ================================================================ SEMANTICS ================================================================
    // ########### IMPL ########### 
    // PLAN [OUTPUT REPRESENTATION]
    // @brief Compiler Output, which means sequential execution
    template <typename NodeT>
    struct SequentialPlan;
    template <typename... NodeTs>
    struct SequentialPlan<NodePack<NodeTs...>>
    {
        using type = NodePack<NodeTs...>;
    };

    // GraphIRT -> SequentialPlan<NodePack<Node<>,Node<>,...>>
    template <typename GraphIRT, ResolverDirection Direction>
    struct MakeSequentialPlanFromIR
    {
        using relations = typename GraphIRT::relations;
        using nodes = typename CollectNodes<relations>::type;

        using type = SequentialPlan<typename TopologicalSortImpl<NodePack<>, nodes, relations, Direction>::type>;
    };

    template <typename GraphT, ResolverDirection Direction>
    struct MakeSequentialPlan
    {
        using ir_graph = typename Lower<GraphT>::type;
        using type = typename MakeSequentialPlanFromIR<ir_graph, Direction>::type;
    };

    template <typename Tag,typename NodePackT, typename RelationPackT, ResolverDirection Direction>
    struct MakeSequentialPlan<GraphIR<Tag, NodePackT, RelationPackT>, Direction>
    {
        using type = typename MakeSequentialPlanFromIR<GraphIR<Tag, NodePackT, RelationPackT>, Direction>::type;
    };

    // @brief: Representing a plan using subgraphs in the case of a multi-layered graph
    template <typename GraphTag, typename GraphT, typename PlanT>
    struct SubPlan
    {
        using tag_type = GraphTag;
        using graph_type = GraphT;
        using plan_type = PlanT;
    };

    // Graph<Tag, ElemenTs..., Direction> -> SubPlan<Graph<Tag, ElemenTs....>, SequentialPlan<Nodepack<Node<...>>>
    template <typename GraphT, ResolverDirection Direction>
    struct MakeSubPlan;
    template <typename GraphTag, ElementType... ElementTs, ResolverDirection Direction>
    struct MakeSubPlan<Graph<GraphTag, ElementTs...>, Direction>
    {
        using graph_type = Graph<GraphTag, ElementTs...>;

        using ir = Lower<Graph<GraphTag, ElementTs...>>;
        using plan_type = typename MakeSequentialPlan<typename ir::type, Direction>::type;

        using type = SubPlan<typename ir::tag, graph_type, plan_type>;
    };

    // @brief A hierarchical execution plan
    template <typename MetaPlan, typename SubPlanPackT>
    struct HierarchicalPlan
    {
        using meta_plan_type = MetaPlan;
        using sub_plan_pack_type = SubPlanPackT;
    };

    // thin wrapper
    template <typename... PlanTs>
    struct SubPlanPack
    {
    };
    template <typename... GraphTs>
    struct GraphPack
    {
    };

    // append
    template <typename ResultPack, typename... PlanTs>
    struct AppendSubPlan;
    template <typename... ExistingTs, typename... PlanTs>
    struct AppendSubPlan<SubPlanPack<ExistingTs...>, PlanTs...>
    {
        using type = SubPlanPack<ExistingTs..., PlanTs...>;
    };

    // fwd
    template<typename GraphT, ResolverDirection Direction>
    struct GraphDirection;
    template<typename GraphT, ResolverDirection DefaultDirection, typename... Overrides>
    struct ResolveGraphDirection
    {
        static constexpr ResolverDirection value = DefaultDirection;
    };
    template<typename TargetGraphT, ResolverDirection DefaultDirection, typename OverrideGraphT, ResolverDirection OverrideDirection, typename... Rest>
    struct ResolveGraphDirection<TargetGraphT, DefaultDirection, GraphDirection<OverrideGraphT, OverrideDirection>, Rest...>
    {
        static constexpr ResolverDirection value = 
            std::is_same_v<TargetGraphT, OverrideGraphT>
                    ? OverrideDirection
                    : ResolveGraphDirection<
                            TargetGraphT,
                            DefaultDirection,
                            Rest...>
                            ::value;
    };

    template<typename MetaPlanT, ResolverDirection DefaultDirection, typename... Overrides>
    struct MakeSubPlanPackFromMetaPlan;

    template<typename... GraphTs, ResolverDirection DefaultDirection, typename... Overrides>
    struct MakeSubPlanPackFromMetaPlan<SequentialPlan<NodePack<Node<GraphTs>...>>, DefaultDirection, Overrides...>
    {
        using type = SubPlanPack<typename MakeSubPlan<GraphTs, ResolveGraphDirection<GraphTs, DefaultDirection, Overrides...>::value>::type...>;
    };


    // ########### API ###########
    template<typename GraphT, ResolverDirection Direction>
    struct GraphDirection
    {
        using graph_type = GraphT;
        static constexpr ResolverDirection direction = Direction;
    };

    template<typename MetaGraphT, ResolverDirection MetaDirection/*meta*/, ResolverDirection DefaultSubDirection/*default sub*/, typename... Overrides/*overrides sub graph direction*/>
    struct MakeHierarchicalPlan
    {
        using meta_ir = typename Lower<MetaGraphT>::type;
        using meta_plan = typename MakeSequentialPlan<meta_ir, MetaDirection>::type;

        using sub_plan_pack = typename MakeSubPlanPackFromMetaPlan<meta_plan, DefaultSubDirection, Overrides...>::type;
        using type = HierarchicalPlan<meta_plan, sub_plan_pack>;
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


    // ================================================================ Layered Plan ================================================================
    // 
    template<typename NodePackT>
    struct LayerPack{};
    template<typename... NodeTs>
    struct LayerPack<NodePack<NodeTs...>>
    {
        using type = NodePack<NodeTs...>;
    };

    // @brief Planning the execution of parallel layers
    template <typename... PlanTs>
    struct LayeredPlan;

    // Impl
    template<typename GraphIRT, ResolverDirection Direction>
    struct MakeLayeredPlanFromIR{};
    // template<typename GraphIRT, ResolverDirection Direction>
    // struct MakeLayeredPlanFromIR<GraphIRT, Direction>
    // {
    //     using type = LayeredPlan<>;
    // };

    // API


    template<typename GraphT, ResolverDirection Direction>
    struct MakeLayeredPlan
    {
        using ir_graph = Lower<GraphT>::type;

        using type = typename MakeLayeredPlanFromIR<ir_graph, Direction>::type;
    };
} // namespace tsr