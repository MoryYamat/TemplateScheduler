#pragma once

#include <cstddef>

#include "tsr/plan/plan.hpp"

namespace tsr
{
    // ******************* PlanStats *******************
    // template <typename... Layers>// fwd
    // struct LayerPack;
    // template <typename LayerPackT>// fwd
    // struct SafeLayeredPlan;
    // template <typename... Nodes>
    // struct NodePack;

    // ##### helpers #####
    template<typename NodePackT>
    struct NodeCounter;
    template<typename... NodeTs>
    struct NodeCounter<NodePack<NodeTs...>>
    {
        static constexpr std::size_t value = sizeof...(NodeTs);
    };

    constexpr std::size_t static_max_size(){return 0;}
    constexpr std::size_t static_max_size(std::size_t x){ return x;}
    template<typename... Rest>
    constexpr std::size_t static_max_size(std::size_t x, Rest... rest)
    {
        auto y = static_max_size(rest...);
        return x > y ? x : y;
    }
    template<typename PlanT>
    struct PlanStats;
    template<typename... Layers>
    struct PlanStats<SafeLayeredPlan<LayerPack<Layers...>>>
    {
        static constexpr std::size_t layer_count = sizeof...(Layers);

        static constexpr std::size_t task_count = (NodeCounter<Layers>::value + ... + 0);
        
        static constexpr std::size_t max_layer_width = static_max_size(NodeCounter<Layers>::value...);

        static constexpr bool has_parallel_layer = max_layer_width > 1;
        static constexpr bool is_fully_sequential = max_layer_width <= 1;
    };
}