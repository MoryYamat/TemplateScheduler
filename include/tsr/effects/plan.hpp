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
    
    template<typename A, typename B>
    inline constexpr bool CanRunTogether_v = CanRunTogether<A,B>::value;

    template <typename ExecutionSetT>
    struct MakeSafeLayredPlan
    {
        // using type = SafeLayeredPlan<>;
    };
} // namespace tsr