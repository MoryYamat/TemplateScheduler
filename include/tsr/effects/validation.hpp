/*
Precondition Check
This verifies the specifications and prerequisites for EffectSystem / ExecutionSet.
*/

#pragma once

#include <type_traits>

#include "tsr/graph/graph_dsl.hpp"
#include "tsr/effects/effect.hpp"
// #include "tsr/graph/graph_dsl.hpp"
#include "tsr/ir/relation.hpp"

namespace tsr
{
    // ****** Duplicate check within ExecutionSet ******
    template <typename ExecutionSet>
    struct ValidateExecutionSetUnique : std::true_type
    {
    };
    template <typename FirstNodeT, typename... RestNodeTs>
    struct ValidateExecutionSetUnique<ExecutionSet<FirstNodeT, RestNodeTs...>>
        : std::bool_constant<(!std::is_same_v<FirstNodeT, RestNodeTs> && ...) &&
                             ValidateExecutionSetUnique<ExecutionSet<RestNodeTs...>>::value>
    {
    };

    // ****** Duplicate check within ResourcePack ******
    template <typename ResourcePackT>
    struct ValidateResourcePackUnique : std::true_type
    {
    };
    template <typename FirstResource, typename... RestResourceTs>
    struct ValidateResourcePackUnique<ResourcePack<FirstResource, RestResourceTs...>>
        : std::bool_constant<(!std::is_same_v<FirstResource, RestResourceTs> && ...) &&
                             ValidateResourcePackUnique<ResourcePack<RestResourceTs...>>::value>
    {
    };

    template <typename T>
    struct ValidateEffects;
    template <typename T>
    struct ValidateEffects<Effects<T>>
        : std::bool_constant<ValidateResourcePackUnique<typename Effects<T>::reads>::value &&
                             ValidateResourcePackUnique<typename Effects<T>::writes>::value>
    {
    };

    template <typename ExecutionSetT>
    struct ValidateExecutionSetEffects;

    template <>
    struct ValidateExecutionSetEffects<ExecutionSet<>> : std::true_type
    {
    };

    template <typename T, typename... RestNodeTs>
    struct ValidateExecutionSetEffects<ExecutionSet<Node<T>, RestNodeTs...>>
        : std::bool_constant<ValidateEffects<Effects<T>>::value &&
                             ValidateExecutionSetEffects<ExecutionSet<RestNodeTs...>>::value>
    {
    };

    // ****** EffectCompleteness ******
    template <typename T, typename = void>
    struct HasEffects : std::false_type
    {
    };
    template <typename T>
    struct HasEffects<T, std::void_t<typename Effects<T>::reads, typename Effects<T>::writes>> : std::true_type
    {
    };
    template <typename ExecutionSetT>
    struct ValidateEffectsCompleteness;
    template <>
    struct ValidateEffectsCompleteness<ExecutionSet<>> : std::true_type
    {
    };
    template <typename T, typename... RestNodeTs>
    struct ValidateEffectsCompleteness<ExecutionSet<Node<T>, RestNodeTs...>>
        : std::bool_constant<HasEffects<T>::value && ValidateEffectsCompleteness<ExecutionSet<RestNodeTs...>>::value>
    {
    };

    // ****** Is unique planNodes ******
    template <typename... Layers>// fwd
    struct LayerPack;
    template <typename LayerPackT>// fwd
    struct SafeLayeredPlan;

    template <typename NodePackT>
    struct ValidateNodePackUnique : std::true_type
    {
    };
    template <typename FirstNodeT, typename... RestNodeTs>
    struct ValidateNodePackUnique<NodePack<FirstNodeT, RestNodeTs...>>
        : std::bool_constant<(!std::is_same_v<FirstNodeT, RestNodeTs> && ...) &&
                             ValidateNodePackUnique<NodePack<RestNodeTs...>>::value>
    {
    };

    // Flatten from layer pack to node pack
    template <typename LayerPackT>
    struct FlattenLayerPack;
    template <>
    struct FlattenLayerPack<LayerPack<>>
    {
        using type = NodePack<>;
    };
    template <typename... NodeTs>
    struct FlattenLayerPack<LayerPack<NodePack<NodeTs...>>>
    {
        using type = NodePack<NodeTs...>;
    };
    template <typename... FirstLayerNodes, typename... SecondLayerNodes, typename... RestLayers>
    struct FlattenLayerPack<LayerPack<NodePack<FirstLayerNodes...>, NodePack<SecondLayerNodes...>, RestLayers...>>
    {
        using type = typename FlattenLayerPack<
            LayerPack<NodePack<FirstLayerNodes..., SecondLayerNodes...>, RestLayers...>>::type;
    };

    template <typename PlanT>
    struct ValidatePlanNodesUnique;
    template <typename LayerPackT>
    struct ValidatePlanNodesUnique<SafeLayeredPlan<LayerPackT>>
        : ValidateNodePackUnique<typename FlattenLayerPack<LayerPackT>::type>
    {
    };

} // namespace tsr