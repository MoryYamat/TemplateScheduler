#pragma once

#include <type_traits>
#include "tsr/effects/effect.hpp"

#include "tsr/plan/plan.hpp"

namespace tsr
{
    // ==================================== SafeLayeredPlan ====================================
    template <typename LayerPackT>
    struct SafeLayeredPlan
    {
        using layer_pack_type = LayerPackT;
    };

    // *********** Extractors ***********
    // reads
    template <typename T>
    struct ExtractReads;
    template <typename T>
    struct ExtractReads<Node<T>>
    {
        using type = typename ExtractReads<Effects<T>>::type;
    };
    template <typename T>
    struct ExtractReads<Effects<T>>
    {
        using type = typename Effects<T>::reads;
    };

    // wirtes
    template <typename T>
    struct ExtractWrites;
    template <typename T>
    struct ExtractWrites<Node<T>>
    {
        using type = typename ExtractWrites<Effects<T>>::type;
    };
    template <typename T>
    struct ExtractWrites<Effects<T>>
    {
        using type = typename Effects<T>::writes;
    };

    // *********** Intersection ************
    template <typename SetA, typename SetB>
    struct ResourcesOverlap;
    template <typename... A, typename... B>
    struct ResourcesOverlap<ResourcePack<A...>, ResourcePack<B...>>
        : std::bool_constant<(false || ... || Contains<A, B...>::value)>
    {
    };

    template <typename A, typename B>
    struct HasWriteWriteConflict;
    template <typename A, typename B>
    struct HasWriteWriteConflict<Node<A>, Node<B>>
    {
        using A_Writes = typename ExtractWrites<Effects<A>>::type;
        using B_Writes = typename ExtractWrites<Effects<B>>::type;
        using type = ResourcesOverlap<A_Writes, B_Writes>;
    };
    template <typename A, typename B>
    struct HasWriteWriteConflict<Effects<A>, Effects<B>>
    {
        using A_Writes = typename ExtractWrites<Effects<A>>::type;
        using B_Writes = typename ExtractWrites<Effects<B>>::type;
        using type = ResourcesOverlap<A_Writes, B_Writes>;
    };

    template <typename A, typename B>
    struct HasReadWriteConflict;
    template <typename A, typename B>
    struct HasReadWriteConflict<Node<A>, Node<B>>
    {
        using A_reads = typename ExtractReads<Effects<A>>::type;
        using B_writes = typename ExtractWrites<Effects<B>>::type;
        using type = ResourcesOverlap<A_reads, B_writes>;
    };
    template <typename A, typename B>
    struct HasReadWriteConflict<Effects<A>, Effects<B>>
    {
        using A_reads = typename ExtractReads<Effects<A>>::type;
        using B_writes = typename ExtractWrites<Effects<B>>::type;
        using type = ResourcesOverlap<A_reads, B_writes>;
    };

    template <typename A, typename B>
    struct CanRunTogether : std::false_type
    {
    };
    template <typename A, typename B>
    struct CanRunTogether<Node<A>, Node<B>> : std::bool_constant<!HasReadWriteConflict<Node<B>, Node<A>>::type::value &&
                                                                 !HasReadWriteConflict<Node<A>, Node<B>>::type::value &&
                                                                 !HasWriteWriteConflict<Node<A>, Node<B>>::type::value>
    {
    };

    template <typename A, typename B>
    inline constexpr bool CanRunTogether_v = CanRunTogether<A, B>::value;

    // *********** Collect Confilicts For Nodes ***********
    template <typename A, typename B>
    struct HasExecutionConflict : std::bool_constant<!CanRunTogether<A, B>::value>
    {
    };

    template<typename ResultRelationPackT, typename TargetNode, typename CurrentNode>
    struct AppendConflictRelation;    
    template<typename... ResultTs, typename TargetNodeT, typename CurrentNodeT>
    struct AppendConflictRelation<RelationPack<ResultTs...>, TargetNodeT, CurrentNodeT>
    {
        using type = 
            std::conditional_t<HasExecutionConflict<TargetNodeT, CurrentNodeT>::value, RelationPack<ResultTs..., Relation<TargetNodeT, CurrentNodeT>>, RelationPack<ResultTs...>>;
    };
    
    template <typename ResultRelationPackT, typename CurrentNodeT, typename RestNodePackT>
    struct CollectConflictsForNode;
    template <typename ResultRelationPackT, typename CurrentNodeT>
    struct CollectConflictsForNode<ResultRelationPackT, CurrentNodeT, ExecutionSet<>>
    {
        using type = ResultRelationPackT;
    };
    template <typename ResultRelationPackT, typename CurrentNodeT, typename FirstRestNodeT, typename... RestNodeTs>
    struct CollectConflictsForNode<ResultRelationPackT, CurrentNodeT, ExecutionSet<FirstRestNodeT, RestNodeTs...>>
    {
        using next_result = typename AppendConflictRelation<ResultRelationPackT, CurrentNodeT, FirstRestNodeT>::type;
        using type = typename CollectConflictsForNode<next_result, CurrentNodeT, ExecutionSet<RestNodeTs...>>::type;
    };
    
    template<typename ResultRelationPackT, typename ExecutionSetT>
    struct CollectConflictRelations;
    template<typename ResultRelationPackT>
    struct CollectConflictRelations<ResultRelationPackT, ExecutionSet<>>
    {
        using type = ResultRelationPackT;
    };
    template<typename ResultRelationPackT, typename FirstNodeT, typename... RestNodeTs>
    struct CollectConflictRelations<ResultRelationPackT, ExecutionSet<FirstNodeT, RestNodeTs...>>
    {
        using next_result = typename CollectConflictsForNode<ResultRelationPackT, FirstNodeT, ExecutionSet<RestNodeTs...>>::type;
        using type = typename CollectConflictRelations<next_result, ExecutionSet<RestNodeTs...>>::type;
    };

    template<typename ExecutionSetT>
    struct ExecutionSetToNodePack;
    template<typename... NodeTs>
    struct ExecutionSetToNodePack<ExecutionSet<NodeTs...>>
    {
        using type = NodePack<NodeTs...>;
    };

    template<typename LayeredPlanT>
    struct ToSafeLayeredPlan;
    template<typename LayeredPackT>
    struct ToSafeLayeredPlan<LayeredPlan<LayeredPackT>>
    {
        using type = SafeLayeredPlan<LayeredPackT>;
    };

    template <typename EffectTag, typename ExecutionSetT, ResolverDirection Direction = ResolverDirection::RootFirst>
    struct MakeSafeLayeredPlan
    {
        using nodes = typename ExecutionSetToNodePack<ExecutionSetT>::type;
        using relations = typename CollectConflictRelations<RelationPack<>, ExecutionSetT>::type;
        using ir = GraphIR<EffectTag, nodes, relations>;

        using layered_plan = typename MakeLayeredPlan<ir, Direction>::type;

        using type = typename ToSafeLayeredPlan<layered_plan>::type;
    };
} // namespace tsr