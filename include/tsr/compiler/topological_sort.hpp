/*
A pass that generates NodePack<...> in execution order from IR.
*/

#pragma once

#include <type_traits>

// #include "tsr/graph/graph_dsl.hpp"
#include "tsr/ir/relation.hpp"

namespace tsr
{
    // ==================== Topological Sort ======================
    template <typename Pack, typename T>
    struct ContainsInPack;
    template <typename... Nodes, typename NodeT>
    struct ContainsInPack<NodePack<Nodes...>, NodeT> : Contains<NodeT, Nodes...>
    {
    };

    template <typename Pack, typename NodeT>
    struct AppendUnique;
    template <typename... NodeTs, typename NodeT>
    struct AppendUnique<NodePack<NodeTs...>, NodeT>
    {
        using type = std::conditional_t<ContainsInPack<NodePack<NodeTs...>, NodeT>::value, NodePack<NodeTs...>,
                                        NodePack<NodeTs..., NodeT>>;
    };

    template <typename NodePack, typename RelationT>
    struct AppendRelationNodes;
    template <typename NodePack, typename A, typename B>
    struct AppendRelationNodes<NodePack, Relation<A, B>>
    {
        using with_a = typename AppendUnique<NodePack, A>::type;
        using type = typename AppendUnique<with_a, B>::type;
    };

    template <typename NodePack, typename RelationPackT>
    struct CollectNodesImpl;
    template <typename Pack>
    struct CollectNodesImpl<Pack, RelationPack<>>
    {
        using type = Pack;
    };
    template <typename NodePack, typename R, typename... Rest>
    struct CollectNodesImpl<NodePack, RelationPack<R, Rest...>>
    {
        using next = typename AppendRelationNodes<NodePack, R>::type;
        using type = typename CollectNodesImpl<next, RelationPack<Rest...>>::type;
    };

    template <typename RelationPackT>
    struct CollectNodes;
    template <typename... Relations>
    struct CollectNodes<RelationPack<Relations...>>
    {
        using type = typename CollectNodesImpl<NodePack<>, RelationPack<Relations...>>::type;
    };

    enum class ResolverDirection : std::uint32_t
    {
        RootFirst,
        LeafFirst
    };

    // For Sort Algorithms
    template <typename NodeT, typename Relation, ResolverDirection Direction = ResolverDirection::RootFirst>
    struct HasPredecessorImpl : std::false_type
    {
    };
    template <typename NodeT, NodeType A, NodeType B>
    struct HasPredecessorImpl<NodeT, Relation<A, B>, ResolverDirection::RootFirst>
        : std::bool_constant<(std::is_same_v<NodeT, B>)>
    {
    }; // check if a Predecessor exists in NodeT.
    template <typename NodeT, NodeType A, NodeType B>
    struct HasPredecessorImpl<NodeT, Relation<A, B>, ResolverDirection::LeafFirst>
        : std::bool_constant<(std::is_same_v<NodeT, A>)>
    {
    }; // check if a Predecessor exists in NodeT(Leaf First).

    template <typename NodeT, typename RelationPackT, ResolverDirection Direction = ResolverDirection::RootFirst>
    struct HasPredecessor;
    template <typename NodeT, ResolverDirection Direction, typename... Relations>
    struct HasPredecessor<NodeT, RelationPack<Relations...>, Direction>
        : std::bool_constant<(HasPredecessorImpl<NodeT, Relations, Direction>::value || ...)>
    {
    };

    // Collect nodes that do not have a preceding node
    template <typename ResultPack, typename NodeT, typename RelationPackT, ResolverDirection Direction>
    struct AppendIfNoPredecessorNodes;

    template <typename ResultPack, NodeType NodeT, typename RelationPackT, ResolverDirection Direction>
    struct AppendIfNoPredecessorNodes<ResultPack, NodeT, RelationPackT, Direction>
    {
        using type = std::conditional_t<HasPredecessor<NodeT, RelationPackT, Direction>::value, ResultPack,
                                        typename AppendUnique<ResultPack, NodeT>::type>;
    };

    template <typename ResultPackT, typename NodePackT, typename RelationPackT, ResolverDirection Direction>
    struct CollectReadyNodesImpl;
    template <typename ResultPackT, typename RelationPackT, ResolverDirection Direction>
    struct CollectReadyNodesImpl<ResultPackT, NodePack<>, RelationPackT, Direction> // termination condition
    {
        using type = ResultPackT;
    };
    template <typename ResultPackT, typename HeadNodeT, typename... RestNodeTs, typename RelationPackT,
              ResolverDirection Direction>
    struct CollectReadyNodesImpl<ResultPackT, NodePack<HeadNodeT, RestNodeTs...>, RelationPackT, Direction>
    {
        using next_result = typename AppendIfNoPredecessorNodes<ResultPackT, HeadNodeT, RelationPackT, Direction>::type;
        using type =
            typename CollectReadyNodesImpl<next_result, NodePack<RestNodeTs...>, RelationPackT, Direction>::type;
    };

    template <typename NodePackT, typename RelationPackT, ResolverDirection Direction>
    struct CollectReadyNodes;
    template <typename... NodeTs, typename RelationPackT, ResolverDirection Direction>
    struct CollectReadyNodes<NodePack<NodeTs...>, RelationPackT, Direction>
    {
        using type = typename CollectReadyNodesImpl<NodePack<>, NodePack<NodeTs...>, RelationPackT, Direction>::type;
    };

    // ============ Remove Relations ============
    // append
    template <typename PackT, typename RelationT>
    struct AppendRelation;
    template <typename... ExistingTs, typename RelationT>
    struct AppendRelation<RelationPack<ExistingTs...>, RelationT>
    {
        using type = RelationPack<ExistingTs..., RelationT>;
    };

    // predicate
    template <typename NodeT, typename RelationT, ResolverDirection Direction>
    struct ShouldRemoveRelation;
    template <typename NodeT, typename A, typename B>
    struct ShouldRemoveRelation<NodeT, Relation<A, B>, ResolverDirection::RootFirst>
        : std::bool_constant<std::is_same_v<NodeT, A>>
    {
    };
    template <typename NodeT, typename A, typename B>
    struct ShouldRemoveRelation<NodeT, Relation<A, B>, ResolverDirection::LeafFirst>
        : std::bool_constant<std::is_same_v<NodeT, B>>
    {
    };

    // fileter
    template <typename ResultPackT, typename NodeT, typename RelationPackT, ResolverDirection Direction>
    struct PruneRelationsByNode;
    template <typename ResultPackT, typename NodeT, ResolverDirection Direction>
    struct PruneRelationsByNode<ResultPackT, NodeT, RelationPack<>, Direction>
    {
        using type = ResultPackT;
    };
    template <typename ResultPackT, typename NodeT, typename FirstRelationT, typename... RestRelationTs,
              ResolverDirection Direction>
    struct PruneRelationsByNode<ResultPackT, NodeT, RelationPack<FirstRelationT, RestRelationTs...>, Direction>
    {
        using next_result = std::conditional_t<ShouldRemoveRelation<NodeT, FirstRelationT, Direction>::value,
                                               ResultPackT, typename AppendRelation<ResultPackT, FirstRelationT>::type>;
        using type =
            typename PruneRelationsByNode<next_result, NodeT, RelationPack<RestRelationTs...>, Direction>::type;
    };

    template <typename NodePackT, typename CurrentRelationPackT, ResolverDirection Direction>
    struct PruneRelationsByNodes;
    template <typename CurrentRelationPackT, ResolverDirection Direction>
    struct PruneRelationsByNodes<NodePack<>, CurrentRelationPackT, Direction>
    {
        using type = CurrentRelationPackT;
    };
    template <typename FirstNodeT, typename... RestNodeTs, typename CurrentRelationPackT, ResolverDirection Direction>
    struct PruneRelationsByNodes<NodePack<FirstNodeT, RestNodeTs...>, CurrentRelationPackT, Direction>
    {
        using next_relations =
            typename PruneRelationsByNode<RelationPack<>, FirstNodeT, CurrentRelationPackT, Direction>::type;
        using type = typename PruneRelationsByNodes<NodePack<RestNodeTs...>, next_relations, Direction>::type;
    };

    // ============ remove nodes ============
    // append
    template <typename PackT, typename NodeT>
    struct AppendNode;
    template <typename... ExistingTs, typename NodeT>
    struct AppendNode<NodePack<ExistingTs...>, NodeT>
    {
        using type = NodePack<ExistingTs..., NodeT>;
    };

    // predicate
    template <typename TargetNodeT, typename NodePackT>
    struct ShouldRemoveNode : std::true_type
    {
    };
    template <typename TargetNodeT, typename... NodeTs>
    struct ShouldRemoveNode<TargetNodeT, NodePack<NodeTs...>>
        : std::bool_constant<(std::is_same_v<TargetNodeT, NodeTs> || ...)>
    {
    };

    // filter
    template <typename ResultPackT /*accum*/, typename ReadyNodePackT /*List to delete*/,
              typename CurrentNodePackT /*intermediate results*/>
    struct PruneNodeByNodes;
    template <typename ResultPackT, typename ReadyNodePackT>
    struct PruneNodeByNodes<ResultPackT, ReadyNodePackT, NodePack<>>
    {
        using type = ResultPackT;
    };
    template <typename ResultPackT, typename ReadyNodePackT, typename FirstNodeT, typename... RestNodeTs>
    struct PruneNodeByNodes<ResultPackT, ReadyNodePackT, NodePack<FirstNodeT, RestNodeTs...>>
    {
        using next_result = std::conditional_t<ShouldRemoveNode<FirstNodeT, ReadyNodePackT>::value, ResultPackT,
                                               typename AppendNode<ResultPackT, FirstNodeT>::type>;

        using type = typename PruneNodeByNodes<next_result, ReadyNodePackT, NodePack<RestNodeTs...>>::type;
    };

    // ============= TOPO SORT IMPL =============
    template <typename OrderPackT, typename ReadyPackT>
    struct ConcatNodePack;
    template <typename... OrderedNodeTs, typename... ReadyNodeTs>
    struct ConcatNodePack<NodePack<OrderedNodeTs...>, NodePack<ReadyNodeTs...>>
    {
        using type = NodePack<OrderedNodeTs..., ReadyNodeTs...>;
    };

    template <typename OrderPackT /*Confirmed topo order*/, typename RemainingNodePackT,
              typename RemainingRelationPackT, ResolverDirection Direction>
    struct TopologicalSortImpl;
    template <typename OrderPackT /*Confirmed topo order*/, typename RemainingRelationPackT,
              ResolverDirection Direction>
    struct TopologicalSortImpl<OrderPackT, NodePack<>, RemainingRelationPackT, Direction> // termination condition
    {
        using type = OrderPackT;
    };
    template <typename OrderPackT /*Confirmed topo order*/, typename... RemainingNodeTs,
              typename RemainingRelationPackT, ResolverDirection Direction>
    struct TopologicalSortImpl<OrderPackT, NodePack<RemainingNodeTs...>, RemainingRelationPackT, Direction>
    {
        // Collect nodes that do not have a preceding node
        using ready = typename CollectReadyNodes<NodePack<RemainingNodeTs...>, RemainingRelationPackT, Direction>::type;

        // simple detection of the graph cylcle or non-topologically sotrtable
        static_assert(!std::is_same_v<ready, NodePack<>>, "Cycle detected or graph is not topologically sortable");

        using next_order = typename ConcatNodePack<OrderPackT, ready>::type; // concat

        using next_nodes = typename PruneNodeByNodes<NodePack<>, ready, NodePack<RemainingNodeTs...>>::type;

        using next_relations = typename PruneRelationsByNodes<ready, RemainingRelationPackT, Direction>::type;

        using type = typename TopologicalSortImpl<next_order, next_nodes, next_relations, Direction>::type;
    };

    template <typename GraphIRT, ResolverDirection Direction = ResolverDirection::RootFirst>
    struct TopologicalSort
    {
        using relations = typename GraphIRT::relations;
        using nodes = typename CollectNodes<relations>::type;

        // topological sort
        using type = typename TopologicalSortImpl<NodePack<> /*initial order*/, nodes, relations, Direction>::type;
    };

    // ================================================================ SEMANTICS ================================================================
    // PLAN [OUTPUT REPRESENTATION]
    // @brief Compiler Output, which means sequential execution
    template <typename NodeT>
    struct SequentialPlan;
    template <typename... NodeTs>
    struct SequentialPlan<NodePack<NodeTs...>>
    {
        using type = NodePack<NodeTs...>;
    };

    // @brief Planning the execution of parallel layers
    template <typename... PlanTs>
    struct LayeredPlan;

    // GraphIRT -> SequentialPlan<NodePack<Node<>,Node<>,...>>
    template <typename GraphIRT, ResolverDirection Direction>
    struct MakeSequentialPlanFromIR
    {
        using relations = typename GraphIRT::relations;
        using nodes = typename CollectNodes<relations>::type;

        using type = SequentialPlan<typename TopologicalSortImpl<NodePack<>, nodes, relations, Direction>::type>;
    };

    template <typename GraphT, ResolverDirection Direction = ResolverDirection::RootFirst>
    struct MakeSequentialPlan
    {
        using ir_graph = typename Lower<GraphT>::type;
        using type = typename MakeSequentialPlanFromIR<ir_graph, Direction>::type;
    };

    template <typename Tag, typename RelationPackT, ResolverDirection Direction>
    struct MakeSequentialPlan<GraphIR<Tag, RelationPackT>, Direction>
    {
        using type = typename MakeSequentialPlanFromIR<GraphIR<Tag, RelationPackT>, Direction>::type;
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

    template<typename MetaPlanT, ResolverDirection Direction>
    struct MakeSubPlanPackFromMetaPlan;

    template<typename... GraphTs, ResolverDirection Direction>
    struct MakeSubPlanPackFromMetaPlan<SequentialPlan<NodePack<Node<GraphTs>...>>, Direction>
    {
        using type = SubPlanPack<typename MakeSubPlan<GraphTs, Direction>::type...>;
    };

    template<typename MetaGraphT, ResolverDirection Direction>
    struct MakeHierarchicalPlan
    {
        using meta_ir = typename Lower<MetaGraphT>::type;
        using meta_plan = typename MakeSequentialPlan<meta_ir, Direction>::type;

        using sub_plan_pack = typename MakeSubPlanPackFromMetaPlan<meta_plan, Direction>::type;
        using type = HierarchicalPlan<meta_plan, sub_plan_pack>;
    };

} // namespace tsr

// HierarchicalPlan<
//     SequentialPlan<NodePack<Node<GG_A_Graph>, Node<GG_B_Graph>, Node<GG_C_Graph>>>,
//     SubPlan<GG_A_Graph, SequentialPlan<NodePack<Node<GG_A_R>, Node<GG_A_0>, ...>>>,
//     SubPlan<GG_B_Graph, SequentialPlan<NodePack<Node<GG_B_R>, Node<GG_B_0>, ...>>>,
//     SubPlan<GG_C_Graph, SequentialPlan<NodePack<Node<GG_C_R>, Node<GG_C_0>, ...>>>
// >