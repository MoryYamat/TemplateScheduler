#pragma once

#include <type_traits>
#include <cstddef>

#include "tsr/effects/access_analysis.hpp"
#include "tsr/plan/plan.hpp"
#include "tsr/analysis/plan_stats.hpp"

namespace tsr
{
    template <typename NodePackT>
    struct IsSequentialLayer : std::true_type
    {
    };
    template <typename... NodeTs>
    struct IsSequentialLayer<NodePack<NodeTs...>>
    {
    };

    template <std::size_t LayerCount, std::size_t SequentialLayerCount, std::size_t ParallelLayerCount>
    struct PlanAnalysisResult
    {
        static constexpr std::size_t layer_count = LayerCount;
        static constexpr std::size_t sequential_layer_count = SequentialLayerCount;
        static constexpr std::size_t parallel_layer_count = ParallelLayerCount;

        static constexpr double parallelism_ratio =
            layer_count == 0 ? 0.0 : static_cast<double>(parallel_layer_count) / layer_count;
    };

    template <typename PlanT>
    struct AnalyzeParallelism;
    template <typename... LayerTs>
    struct AnalyzeParallelism<SafeLayeredPlan<LayerPack<LayerTs...>>>
    {
        using type = PlanAnalysisResult<sizeof...(LayerTs),
                                        (std::size_t{0} + ... + (NodeCounter<LayerTs>::value <= 1 ? 1u : 0u)),
                                        (std::size_t{0} + ... + (NodeCounter<LayerTs>::value > 1 ? 1u : 0u))>;
    };

    template <typename PlanT>
    struct AnalyzePlan
    {
        //using type = PlanAnalysisResult<>
    };

    template <typename ConflictRelationTs>
    struct ConflictAnalysisResult
    {
        using conflict_relations = ConflictRelationTs;
    };
    template <typename ExecutionSetT>
    struct AnalyzeConflicts
    {
        using type = ConflictAnalysisResult<typename CollectConflictRelations<RelationPack<>, ExecutionSetT>::type>;
    };

    template <typename ResourceT, typename ReadersT, typename WritersT>
    struct ResourceDependency
    {
        using resource = ResourceT;
        using readers = ReadersT;
        using writers = WritersT;
    };

    template <typename... Dependencies>
    struct ResourceDependencyAnalysisResult
    {
    };

    template <typename ResourcePackT, typename ReadTs, typename WriteTs>
    struct AppendResourcePack;
    template <typename... ResourceTs, typename... ReadTs, typename... WriteTs>
    struct AppendResourcePack<ResourcePack<ResourceTs...>, ResourcePack<ReadTs...>, ResourcePack<WriteTs...>>
    {
        using type = ResourcePack<ResourceTs..., ReadTs..., WriteTs...>;
    };
    template <typename ResultPackT, typename ExecutionSetT>
    struct CollectAllResources;
    template <typename ResultPackT>
    struct CollectAllResources<ResultPackT, ExecutionSet<>>
    {
        using type = ResultPackT;
    };
    template <typename ResultPackT, typename FirstNodeT, typename... RestNodeTs>
    struct CollectAllResources<ResultPackT, ExecutionSet<FirstNodeT, RestNodeTs...>>
    {
        using reads = typename ExtractReads<FirstNodeT>::type;
        using writes = typename ExtractWrites<FirstNodeT>::type;
        using next_result = typename AppendResourcePack<ResultPackT, reads, writes>::type;
        using type = typename CollectAllResources<next_result, ExecutionSet<RestNodeTs...>>::type;
    };

    template <typename ResultPackT, typename TargetNodeT>
    struct AppendResultNode;
    template <typename... ResultNodeTs, typename TargetNodeT>
    struct AppendResultNode<NodePack<ResultNodeTs...>, TargetNodeT>
    {
        using type = NodePack<ResultNodeTs..., TargetNodeT>;
    };

    template <typename TargetT, typename SearchSetT>
    struct ResourcePackContains : std::false_type
    {
    };
    template <typename TargetT, typename... Ts>
    struct ResourcePackContains<TargetT, ResourcePack<Ts...>> : std::bool_constant<(std::is_same_v<TargetT, Ts> || ...)>
    {
    };
    template <typename ResultPackT, typename TargetT>
    struct AppendNode;
    template <typename... ResultNodeTs, typename TargetT>
    struct AppendNode<NodePack<ResultNodeTs...>, TargetT>
    {
        using type = NodePack<ResultNodeTs..., TargetT>;
    };

    template <typename ResultPackT, typename ResourceT, typename ExecutionSetT>
    struct CollectReadersForResource;
    template <typename ResultPackT, typename ResourceT>
    struct CollectReadersForResource<ResultPackT, ResourceT, ExecutionSet<>>
    {
        using type = ResultPackT;
    };
    template <typename ResultPackT, typename ResourceT, typename FirstNodeT, typename... RestNodeTs>
    struct CollectReadersForResource<ResultPackT, ResourceT, ExecutionSet<FirstNodeT, RestNodeTs...>>
    {
        using first_reads = typename ExtractReads<FirstNodeT>::type;
        using next_result = std::conditional_t<ResourcePackContains<ResourceT, first_reads>::value,
                                               typename AppendNode<ResultPackT, FirstNodeT>::type, ResultPackT>;
        using type = CollectReadersForResource<next_result, ResourceT, ExecutionSet<RestNodeTs...>>::type;
    };
    template <typename ResultPackT, typename ResourceT, typename ExecutionSetT>
    struct CollectWritersForResource;
    template <typename ResultPackT, typename ResourceT>
    struct CollectWritersForResource<ResultPackT, ResourceT, ExecutionSet<>>
    {
        using type = ResultPackT;
    };
    template <typename ResultPackT, typename ResourceT, typename FirstNodeT, typename... RestNodeTs>
    struct CollectWritersForResource<ResultPackT, ResourceT, ExecutionSet<FirstNodeT, RestNodeTs...>>
    {
        using first_writes  = typename ExtractWrites<FirstNodeT>::type;
        using next_result = std::conditional_t<ResourcePackContains<ResourceT, first_writes >::value,
                                               typename AppendNode<ResultPackT, FirstNodeT>::type, ResultPackT>;
        using type = CollectWritersForResource<next_result, ResourceT, ExecutionSet<RestNodeTs...>>::type;
    };

    // template<typename ResourceT, typename ExecutionSetT>
    // struct BuildResourceDependency;
    // template<typename ResourceT, typename... NodeTs>
    // struct BuildResourceDependency<ResourceT, ExecutionSet<NodeTs...>>
    // {
    //     using readers = typename CollectReadersForResource<NodePack<>, ResourceT, ExecutionSet<NodeTs...>>::type;
    //     using writers = typename CollectWritersForResource<NodePack<>, ResourceT, ExecutionSet<NodeTs...>>::type;
    //     using type = ResourceDependency<ResourceT, readers, writers>;
    // };

    template <typename ResultPackT, typename TargetT>
    struct AppendResourceDependencyAnalysisResult;
    template <typename... ResultTs, typename TargetT>
    struct AppendResourceDependencyAnalysisResult<ResourceDependencyAnalysisResult<ResultTs...>, TargetT>
    {
        using type = ResourceDependencyAnalysisResult<ResultTs..., TargetT>;
    };

    template <typename ResultPackT, typename ResourcePackT, typename ExecutionSetT>
    struct BuildResourceDependency;
    template <typename ResultPackT, typename ExecutionSetT>
    struct BuildResourceDependency<ResultPackT, ResourcePack<>, ExecutionSetT>
    {
        using type = ResultPackT;
    };
    template <typename ResultTs, typename FirstResourceT, typename... RestResourceTs, typename... NodeTs>
    struct BuildResourceDependency<ResultTs, ResourcePack<FirstResourceT, RestResourceTs...>, ExecutionSet<NodeTs...>>
    {
        using readers = typename CollectReadersForResource<NodePack<>, FirstResourceT, ExecutionSet<NodeTs...>>::type;
        using writers = typename CollectWritersForResource<NodePack<>, FirstResourceT, ExecutionSet<NodeTs...>>::type;
        using next_result =
            typename AppendResourceDependencyAnalysisResult<ResultTs,
                                                            ResourceDependency<FirstResourceT, readers, writers>>::type;
        using type =
            BuildResourceDependency<next_result, ResourcePack<RestResourceTs...>, ExecutionSet<NodeTs...>>::type;
    };

    template <typename ResultPackT, typename ResourceT>
    struct AppendUniqueResource;
    template <typename... ResultTs, typename ResourceT>
    struct AppendUniqueResource<ResourcePack<ResultTs...>, ResourceT>
    {
        using type = std::conditional_t<ResourcePackContains<ResourceT, ResourcePack<ResultTs...>>::value,
                                        ResourcePack<ResultTs...>, ResourcePack<ResultTs..., ResourceT>>;
    };

    template <typename ResultPackT, typename ResourcePackT>
    struct UniqueResourcePackImpl;
    template <typename ResultPackT>
    struct UniqueResourcePackImpl<ResultPackT, ResourcePack<>>
    {
        using type = ResultPackT;
    };
    template <typename ResultPackT, typename FirstResourceT, typename... RestResourceTs>
    struct UniqueResourcePackImpl<ResultPackT, ResourcePack<FirstResourceT, RestResourceTs...>>
    {
        using next_result = typename AppendUniqueResource<ResultPackT, FirstResourceT>::type;

        using type = typename UniqueResourcePackImpl<next_result, ResourcePack<RestResourceTs...>>::type;
    };

    template <typename ResourcePackT>
    struct UniqueResourcePack;
    template <typename... ResourceTs>
    struct UniqueResourcePack<ResourcePack<ResourceTs...>>
    {
        using type = typename UniqueResourcePackImpl<ResourcePack<>, ResourcePack<ResourceTs...>>::type;
    };

    template <typename T>
    struct AnalyzeResourceDependencies;
    template <typename... NodeTs>
    struct AnalyzeResourceDependencies<ExecutionSet<NodeTs...>>
    {
        using raw_resources = typename CollectAllResources<ResourcePack<>, ExecutionSet<NodeTs...>>::type;
        using resources = typename UniqueResourcePack<raw_resources>::type;
        using type = typename BuildResourceDependency<ResourceDependencyAnalysisResult<>, resources,
                                                      ExecutionSet<NodeTs...>>::type;
    };
} // namespace tsr