#pragma once

#include <type_traits>
#include <cstddef>

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

    // seqauential_layer_count: node が 1個だけの layerの数
    // parallel_layer_count:    node が 2個以上 ある layerの数
    // parallelism_ration:      parallel_layer_count / layer_count
    // average_layer_width:
} // namespace tsr