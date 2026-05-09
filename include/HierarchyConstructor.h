#pragma once

#include <iostream>
#include <cstdint>

#include <type_traits>

namespace tsr
{

    inline constexpr int indent_width = 2;

    // Node<T>      // 外部型を包む
    // Arc<R, B...> // R <- B... の関係
    // Graph<Tag, Elements...>
    // ============================================================== DESCRPT ==============================================================
    // ============================================================== DESCRPT ==============================================================
    template <typename T>
    struct Node
    {
        using basic_type = T;
    };

    // *********************** type traits ***********************
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
    // ***********************************************************

    // ###################### TYPE ######################
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
    // ##################################################

    // *********************** type traits ***********************
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
    // ***********************************************************

    // ###################### TYPE ######################
    template <typename Tag, ElementType... ElementTs>
    struct Graph
    {
        static_assert((IsElement<ElementTs>::value && ...), "Graph elements must be Node<T> or Arc<Node<T>, ...>");

        static_assert((IsValidElement<ElementTs>::value && ...),
                      "Graph elements must be valid Node<T> or valid Arc<Node<T>, ...>");

        using tag_type = Tag;
        using element_types = ElementPack<ElementTs...>;

    };
    // ##################################################
    // ==========================================================================================================================================================================================================================================================================

    // ============================================================== COMPILER ==============================================================
    // ============================================================== COMPILER ==============================================================

    // =================== Scanning policy ========================
    enum class ResolverDirection : std::uint32_t
    {
        RootFirst,
        LeafFirst
    };

    // ======================== Resolver ==========================
    // TEST
    // UNIT

    template <typename T, ResolverDirection Direction = ResolverDirection::RootFirst>
    struct NodeResolver
    {
        static void Resolve()
        {
            std::cerr << "Type = " << typeid(T).name() << "\n";
        }
    };
    template <typename T, ResolverDirection Direction>
    struct NodeResolver<Node<T>, Direction>
    {
        static void Resolve()
        {
            NodeResolver<T>::Resolve();
        }
    };

    template <typename Root, typename... ElementTs, ResolverDirection Direction>
    struct NodeResolver<Arc<Root, ElementTs...>, Direction>
    {
        static void Resolve()
        {
            if constexpr (Direction == ResolverDirection::RootFirst)
            {
                NodeResolver<Root, ResolverDirection::RootFirst>::Resolve();
                ((NodeResolver<ElementTs, ResolverDirection::RootFirst>::Resolve()), ...);
            }
            else
            {
                ((NodeResolver<ElementTs, ResolverDirection::LeafFirst>::Resolve()), ...);
                NodeResolver<Root, ResolverDirection::LeafFirst>::Resolve();
            }
        }
    };

    template <typename Tag, typename... ElementTs, ResolverDirection Direction>
    struct NodeResolver<Graph<Tag, ElementTs...>, Direction>
    {
        static void Resolve()
        {
            NodeResolver<Tag, Direction>::Resolve();
            ((NodeResolver<ElementTs, Direction>::Resolve()), ...);
        }
    };
    // UNIT

    template <typename Graph, ResolverDirection Direction = ResolverDirection::RootFirst>
    struct ResolveOrder;

    template <typename Tag, ElementType... ElementTs, ResolverDirection Direction>
    struct ResolveOrder<Graph<Tag, ElementTs...>, Direction>
    {
        static void Resolve()
        {
            if constexpr (Direction == ResolverDirection::RootFirst)
            {
                std::cerr << "RootFirst\n";
                {
                    // NodeResolver<Tag, ResolverDirection::RootFirst>::Resolve();
                    ((NodeResolver<ElementTs, ResolverDirection::RootFirst>::Resolve()), ...);
                    // NodeResolver<Root>::Resolve();
                    // NodeResolver<Child1>::Resolve();
                    // NodeResolver<Child2>::Resolve();
                    // NodeResolver<GrandChild1>::Resolve();
                    // ...
                }
            }
            else
            {
                std::cerr << "LeafFirst\n";
                {
                    // pack
                    // Here, it might be a good idea to pack the items back into the pack (stack) in order to reverse the order
                    // NodeResolver<Tag, ResolverDirection::LeafFirst>::Resolve();
                    ((NodeResolver<ElementTs, ResolverDirection::LeafFirst>::Resolve()), ...);
                    
                    // ...
                    // NodeResolver<GrandChild1>::Resolve();
                    // NodeResolver<Child2>::Resolve();
                    // NodeResolver<Child1>::Resolve();
                    // NodeResolver<Root>::Resolve();
                }
            }
        }
    };
    // ============================================================== COMPILER ==============================================================
    // ============================================================== COMPILER ==============================================================

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