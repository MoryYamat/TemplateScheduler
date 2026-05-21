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

    // Collect Nodes from DSL
    // Extract Nodes From DSL

    // To Unique Nodes
    template <typename TargetNodeT, typename ResultPackT>
    struct Is_UniqueNode : std::true_type
    {
    };
    template <typename TargetNodeT, typename FirstNodeT, typename... RestNodeTs>
    struct Is_UniqueNode<TargetNodeT, NodePack<FirstNodeT, RestNodeTs...>>
        : std::bool_constant<!std::is_same_v<TargetNodeT, FirstNodeT> &&
                             Is_UniqueNode<TargetNodeT, NodePack<RestNodeTs...>>::value>
    {
    };
    template <typename ResultPackT, typename ElementPackT>
    struct UniqueNodePackImpl
    {
    };
    template <typename ResultPackT>
    struct UniqueNodePackImpl<ResultPackT, NodePack<>>
    {
        using type = ResultPackT;
    };
    template <typename... ResultNodeTs, typename FirstNodeT, typename... RestNodeT>
    struct UniqueNodePackImpl<NodePack<ResultNodeTs...>, NodePack<FirstNodeT, RestNodeT...>>
    {
        using next_result = std::conditional_t<Is_UniqueNode<FirstNodeT, NodePack<ResultNodeTs...>>::value,
                                               NodePack<ResultNodeTs..., FirstNodeT>, NodePack<ResultNodeTs...>>;

        using type = typename UniqueNodePackImpl<next_result, NodePack<RestNodeT...>>::type;
    };
    template <typename NodePackT>
    struct UniqueNodePack;
    template <typename... NodeTs>
    struct UniqueNodePack<NodePack<NodeTs...>>
    {
        using type = typename UniqueNodePackImpl<NodePack<>, NodePack<NodeTs...>>::type;
    };

    // Concat
    template <typename... Packs>
    struct ConcatNodePacks;
    template <>
    struct ConcatNodePacks<>
    {
        using type = NodePack<>;
    };
    template <typename... NodeTs>
    struct ConcatNodePacks<NodePack<NodeTs...>>
    {
        using type = NodePack<NodeTs...>;
    };
    template <typename... ANodeTs, typename... BNodeTs, typename... RestPacks>
    struct ConcatNodePacks<NodePack<ANodeTs...>, NodePack<BNodeTs...>, RestPacks...>
    {
        using type = typename ConcatNodePacks<NodePack<ANodeTs..., BNodeTs...>, RestPacks...>::type;
    };

    template <typename ElementTs>
    struct CollectRawNodesFromElement
    {
    };
    template <typename T>
    struct CollectRawNodesFromElement<Node<T>>
    {
        using type = NodePack<Node<T>>;
    };
    template <NodeType RootNodeT, typename... BranchTs>
    struct CollectRawNodesFromElement<Arc<RootNodeT, BranchTs...>>
    {
        using type =
            typename ConcatNodePacks<NodePack<RootNodeT>, typename CollectRawNodesFromElement<BranchTs>::type...>::type;
    };

    template <typename GraphT>
    struct CollectRawNodesFromGraph;
    template <typename Tag, ElementType... ElementTs>
    struct CollectRawNodesFromGraph<Graph<Tag, ElementTs...>>
    {
        using type = typename ConcatNodePacks<typename CollectRawNodesFromElement<ElementTs>::type...>::type;
    };

    template <typename GraphT>
    struct Lower;

    template <typename Tag, ElementType... ElementTs>
    struct Lower<Graph<Tag, ElementTs...>>
    {
        using tag = Tag;

        using raw_nodes = typename CollectRawNodesFromGraph<Graph<Tag, ElementTs...>>::type;

        using nodes = typename UniqueNodePack<raw_nodes>::type;

        using relations = typename Concat<typename ToRelations<ElementTs>::type...>::type;
        using type = GraphIR<Tag, nodes, relations>;
    };
    // ***********************************************************
} // namespace tsr