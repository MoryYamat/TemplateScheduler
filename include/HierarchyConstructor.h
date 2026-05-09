#pragma once

#include <iostream>
#include <cstdint>

#include <type_traits>

namespace tsr
{

    inline constexpr int indent_width = 2;

    // ==================== minimum structure ====================
    template <typename T>
    struct Node
    {
        using basic_type = T;
    };

    // *************** type traits ***************
    template <typename T>
    struct IsNode : std::false_type
    {
    };
    template <typename T>
    struct IsNode<Node<T>> : std::true_type
    {
    };
    template <typename T>
    concept NodeType = IsNode<T>::value;
    // *******************************************

    template <NodeType... Nodes>
    struct NodePack
    {
    };

    template <typename... BranchTs>
    struct ElementPack
    {
    };

    // fwd
    template <typename T>
    struct IsElement;

    template <NodeType RootNodeT, typename... BranchTs> // Arc<Node, Branch...>//
    struct Arc
    {
        static_assert((IsElement<BranchTs>::value && ...), "Arc branches must be Node<T> or valid Arc<Node<T>, ...>");

        using root_node = RootNodeT;
        using branches = ElementPack<BranchTs...>;
    };

    // *************** type traits ***************
    template <typename T>
    struct IsElement : std::false_type
    {
    };
    template <typename NodeT>
    struct IsElement<Node<NodeT>> : std::true_type
    {
    };
    // template<PureNode RootNodeT, typename... BranchTs>
    // struct IsArcBranch<Arc<RootNodeT, BranchTs...>> : std::bool_constant<(IsArcBranch<BranchTs>::value&&...)>{};
    template <NodeType RootNodeT, typename... BranchTs>
    struct IsElement<Arc<RootNodeT, BranchTs...>> : std::true_type
    {
    };
    template <typename T>
    concept ElementType = IsElement<T>::value; // // Node<T> or Arc<...>

    // validate
    template <typename T>
    struct IsValidElement : std::false_type
    {
    };

    template <typename T>
    struct IsValidElement<Node<T>> : std::true_type
    {
    };

    template <NodeType RootNodeT, typename... BranchTs>
    struct IsValidElement<Arc<RootNodeT, BranchTs...>> : std::bool_constant<(IsValidElement<BranchTs>::value && ...)>
    {
    }; // check if BranchTs are Node<T>
    //
    // *******************************************

    template <typename Tag, typename... ElementTs>
    struct Graph
    {
        static_assert((IsElement<ElementTs>::value && ...), "Graph elements must be Node<T> or Arc<Node<T>, ...>");

        static_assert((IsValidElement<ElementTs>::value && ...),
                      "Graph elements must be valid Node<T> or valid Arc<Node<T>, ...>");

        using tag_type = Tag;
        using element_types = ElementPack<ElementTs...>;
    };

    // ============================================================

    // =================== Scanning policy ========================
    enum class ResolverDirection : std::uint32_t
    {
        RootFirst,
        LeafFirst
    };
    // ======================== Resolver ==========================
    template <typename Graph, ResolverDirection Direction>
    struct ResolveOrder;

    template <typename Tag, typename... dependencies, ResolverDirection Direction>
    struct ResolveOrder<Graph<Tag, dependencies...>, Direction>
    {
        static void Resolve()
        {
            if constexpr (Direction == ResolverDirection::RootFirst)
            {
                std::cerr << "RootFirst\n";
            }
            else
            {
                std::cerr << "LeafFirst\n";
            }
        }
    };

    // TEST
    template <typename T>
    struct NodeResolver
    {
        static void Resolve()
        {
            std::cerr << "Type = " << typeid(T).name() << "\n";
        }
    };
    template <typename T>
    struct NodeResolver<Node<T>>
    {
        static void Resolve()
        {
            NodeResolver<T>::Resolve();
        }
    };

    template <typename N, typename... Dependencies>
    struct NodeResolver<Arc<N, Dependencies...>>
    {
        static void Resolve()
        {
            NodeResolver<N>::Resolve();
            ((NodeResolver<Dependencies>::Resolve()), ...);
        }
    };

    template <typename Tag, typename... Dependencies>
    struct NodeResolver<Graph<Tag, Dependencies...>>
    {
        static void Resolve()
        {
            NodeResolver<Tag>::Resolve();
            ((NodeResolver<Dependencies>::Resolve()), ...);
        }
    };

    // ============================================================

    // ==================== TOPOLOGICAL SORT ======================
    // using Order = ResolverOrder<Graph, DependencFirst>::type;

    // ============================================================

    // ====================== REVERSE ORDER =======================

    // ============================================================

    // ===================== CYCLE DETECTION ======================

    // ============================================================

    // ====================== PRALLEL LAYER =======================

    // ============================================================
} // namespace tsr