/*
Users explicitly verify the plan.
*/

#pragma once

// #include "tsr/ir/relation.hpp"
#include "tsr/plan/plan.hpp"
#include "tsr/effects/access_analysis.hpp"
#include <type_traits>

namespace tsr
{
    // ============================= SafeLayered Plan =============================
    // SafeLayeredPlan<LayerPack<NodePac<A,B>,NodePack<C>>>;
    // **** Conflict ****
    // check all pairs in each layer
    template<typename NodePackT>
    struct ValidateLayer;
    template<>
    struct ValidateLayer<NodePack<>> : std::true_type {};
    template<typename T>
    struct ValidateLayer<NodePack<Node<T>>> : std::true_type {};
    template<typename FirstT, typename... RestTs>
    struct ValidateLayer<NodePack<Node<FirstT>, Node<RestTs>...>>
        : std::bool_constant<(CanRunTogether_v<Node<FirstT>, Node<RestTs>> && ...)
          &&
          ValidateLayer<NodePack<Node<RestTs>...>>::value>
        {};

    // apply the ValidateLayer to all Layer
    template<typename LayerPackT>
    struct ValidateLayerPack;
    template<typename... NodePackTs>
    struct ValidateLayerPack<LayerPack<NodePackTs...>>
        : std::bool_constant<(ValidateLayer<NodePackTs>::value && ...)>{};
    
    // wrapper
    template<typename PlanT>
    struct ValidateSafeLayeredPlan;
    template<typename LayerT>
    struct ValidateSafeLayeredPlan<SafeLayeredPlan<LayerT>>
        : ValidateLayerPack<LayerT>
    {};

    template<typename PlanT>
    struct RequireValidSafeLayeredPlan
    {
        static_assert(ValidateSafeLayeredPlan<PlanT>::value, "SafeLayeredPlan contains conflicting nodes in the same layer");
        using type = PlanT;
    };
}