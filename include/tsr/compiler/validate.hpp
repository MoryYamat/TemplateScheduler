/*
Check if the IR meets the prerequisites for the compiler pass.
*/

#pragma once

#include "tsr/ir/graph_ir.hpp"
#include "tsr/ir/relation.hpp"

namespace tsr
{
    // 5. IsUnique<flattened relations...>
    template <typename Pack>
    struct IsUniqueRelations : std::false_type
    {
    };
    template <typename... Relations>
    struct IsUniqueRelations<RelationPack<Relations...>> : IsUnique<Relations...>
    {
    };

    // ============== Bidirectional edge detection ==============
    template <typename A, typename... Relations>
    struct ContainsReverse;
    template <typename A, typename B, typename... Relations>
    struct ContainsReverse<Relation<A, B>, Relations...>
        : std::bool_constant<(std::is_same_v<Relation<B, A>, Relations> || ...)>
    {
    };
    template <typename... Relations>
    struct HasBidirectionalRelation : std::false_type
    {
    };
    template <typename R, typename... Rest>
    struct HasBidirectionalRelation<R, Rest...>
        : std::bool_constant<(ContainsReverse<R, Rest...>::value || HasBidirectionalRelation<Rest...>::value)>
    {
    };
    // for pack
    template <typename Pack>
    struct HasBidirectionalRelationInPack : std::false_type
    {
    };
    template <typename... Relations>
    struct HasBidirectionalRelationInPack<RelationPack<Relations...>> : HasBidirectionalRelation<Relations...>
    {
    };
    // =====================================================================================================

    // compiler/validate.hpp
    template <typename IR>
    struct Validate;

    template <typename Tag, typename Relations>
    struct Validate<GraphIR<Tag, Relations>>
    {
        static_assert(IsUniqueRelations<Relations>::value, "Duplicated relations detected");

        static_assert(!HasBidirectionalRelationInPack<Relations>::value, "Bidirectional relations are not allowed");

        using type = GraphIR<Tag, Relations>;
    };
} // namespace tsr