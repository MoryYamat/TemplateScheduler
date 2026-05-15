/*
Definition of an internal representation that converts a DSL into a form 
that is easy for the compiler to handle, and the conversion process.
*/

#pragma once

#include "tsr/graph/graph_dsl.hpp"
#include "tsr/ir/graph_ir.hpp"

namespace tsr
{
    // *********************** type traits ***********************
    // 1. Relation<Root, Branch>// primitive
    template <NodeType RootNodeT, NodeType Branch>
    struct Relation
    {
    };

    template <typename... Relations>
    struct RelationPack
    {
    };

    // 2. BranchRoot<T>// to extract Arc<Node<RootNode>, Node<BranchTs>...> and Arc<Node<RootNode>, Arc<RootNode,BranchTs...>>
    template <typename T>
    struct ElementRoot;
    template <typename T>
    struct ElementRoot<Node<T>>
    {
        using type = Node<T>; // ElementRoot<Node<T>>::type
    };
    template <NodeType RootNodeT, typename... BranchTs>
    struct ElementRoot<Arc<RootNodeT, BranchTs...>>
    {
        using type = RootNodeT; // ElementRoot<Arc<RootNodeT, BranchTs...>::type == Node<RootNodeT>

        // delete
        // using branchTs =
        //     RelationPack<BranchTs...>; // ElementRoot<Arc<RootNodeT, BranchTs...>::branchTs == ElementPack<BranchTs...>
    };

    // 4. ElementPack の concat
    template <typename... Packs>
    struct Concat;
    template <>
    struct Concat<>
    {
        using type = RelationPack<>;
    };
    template <typename... Ts>
    struct Concat<RelationPack<Ts...>>
    {
        using type = RelationPack<Ts...>;
    };
    template <typename... Ts, typename... Us, typename... Rest>
    struct Concat<RelationPack<Ts...>, RelationPack<Us...>, Rest...>
    {
        using type = typename Concat<RelationPack<Ts..., Us...>, Rest...>::type;
    };

    // ToRelations
    template <typename T>
    struct ToRelations;
    template <typename T>
    struct ToRelations<Node<T>>
    {
        using type = RelationPack<>;
    };
    template <NodeType RootNodeT, typename... BranchTs>
    struct ToRelations<Arc<RootNodeT, BranchTs...>>
    {
        using direct_type = RelationPack<Relation<RootNodeT, typename ElementRoot<BranchTs>::type>...>; // direct

        using nested_type = typename Concat<typename ToRelations<BranchTs>::type...>::type; // nested

        using type = typename Concat<direct_type, nested_type>::type; // concat
    };

    template <typename GraphT>
    struct Lower;

    template <typename Tag, ElementType... ElementTs>
    struct Lower<Graph<Tag, ElementTs...>>
    {
        using relations = typename Concat<typename ToRelations<ElementTs>::type...>::type;

        using type = GraphIR<Tag, relations>;
    };
    // ***********************************************************
} // namespace tsr