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

    // ============================================ Validations ============================================
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

    // Check Duplication
    template <typename T, typename... Ts>
    struct Contains : std::bool_constant<(std::is_same_v<T, Ts> || ...)>
    {
    };
    template <typename... Ts>
    struct IsUnique : std::true_type
    {
    };
    template <typename T, typename... Rest>
    struct IsUnique<T, Rest...> : std::bool_constant<!Contains<T, Rest...>::value && IsUnique<Rest...>::value>
    {
    };
    // =====================================================================================================
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

    // ====================== fwd =========================
    template <typename T>
    struct IsElement;
    // fwd
    // ====================================================

    template <NodeType RootNodeT, typename... BranchTs> // Arc<Node, Branch...>//
    struct Arc
    {
        static_assert((IsElement<BranchTs>::value && ...), "Arc branches must be Node<T> or valid Arc<Node<T>, ...>");

        static_assert(IsUnique<BranchTs...>::value, "Arc elements must be unique");
        using root_node = RootNodeT;
        using branches = ElementPack<BranchTs...>;
    };
    // ##################################################

    
    // ============================================ Validations ============================================
    // *********************** type traits ***********************
    // 1. Relation<Root, Branch>// primitive
    template <NodeType RootNodeT, NodeType Branch>
    struct Relation
    {
        static void Print()
        {
            std::cerr << "Root_Type = " << typeid(RootNodeT).name() << "/ Child_Type = " << typeid(Branch).name()
                      << "\n";
        }
    };

    template<typename... Relations>
    struct RelationPack{};

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
        using direct_type = RelationPack<Relation<RootNodeT, typename ElementRoot<BranchTs>::type>...>;  // direct

        using nested_type = typename Concat<typename ToRelations<BranchTs>::type...>::type;             // nested

        using type = typename Concat<direct_type, nested_type>::type;                                   // concat

        static void Print()
        {
            std::cerr << typeid(type).name() << "\n";
        }
    };
    // 5. IsUnique<flattened relations...>
    template<typename Pack>
    struct IsUniqueRelations : std::false_type{};
    template<typename... Relations>
    struct IsUniqueRelations<RelationPack<Relations...>> : IsUnique<Relations...> {};

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
    // =====================================================================================================
    // ***********************************************************

    // ###################### TYPE ######################
    template <typename Tag, ElementType... ElementTs>
    struct Graph
    {
        static_assert((IsElement<ElementTs>::value && ...), "Graph elements must be Node<T> or Arc<Node<T>, ...>");

        static_assert((IsValidElement<ElementTs>::value && ...),
                      "Graph elements must be valid Node<T> or valid Arc<Node<T>, ...>");

        using relations = typename Concat<typename ToRelations<ElementTs>::type...>::type;
        static_assert(IsUniqueRelations<relations>::value, "Duplicated relations detected");//  check relational duplication

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

    // helpers
    template <ResolverDirection Direction>
    inline constexpr void PrintResolverDirection()
    {
        if constexpr (Direction == ResolverDirection::RootFirst)
        {
            std::cerr << "Root First\n";
        }
        else
        {
            std::cerr << "Leaf First\n";
        }
    }

    template <typename Tag>
    struct TagResolver
    {
        static void Resolve()
        {
            std::cerr << "Graph Type = " << typeid(Tag).name() << "\n";
        }
    };

    void indent(int indent = 0)
    {
        for (int i = 0; i < indent; i++)
            std::cerr << "-";
    }

    // ======================== Resolver ==========================
    // TEST
    // UNIT
    template <typename T, ResolverDirection Direction = ResolverDirection::RootFirst>
    struct NodeResolver
    {
        static void Resolve(int i)
        {
            indent(i);
            std::cerr << "Type = " << typeid(T).name() << "\n";
        }

        static void Resolve()
        {
            std::cerr << "Type = " << typeid(T).name() << "\n";
        }
    };
    template <typename T, ResolverDirection Direction>
    struct NodeResolver<Node<T>, Direction>
    {
        static void Resolve(int i)
        {
            NodeResolver<T, Direction>::Resolve(i);
        }

        static void Resolve()
        {
            NodeResolver<T, Direction>::Resolve();
        }
    };
    template <typename Root, typename... ElementTs, ResolverDirection Direction>
    struct NodeResolver<Arc<Root, ElementTs...>, Direction>
    {
        static void Resolve(int i)
        {
            if constexpr (Direction == ResolverDirection::RootFirst)
            {
                NodeResolver<Root, ResolverDirection::RootFirst>::Resolve(i);
                ((NodeResolver<ElementTs, ResolverDirection::RootFirst>::Resolve(i + 4)), ...);
            }
            else
            {
                ((NodeResolver<ElementTs, ResolverDirection::LeafFirst>::Resolve(i + 4)), ...);
                NodeResolver<Root, ResolverDirection::LeafFirst>::Resolve(i);
            }
        }

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
        static void Resolve(int i)
        {
            TagResolver<Tag>::Resolve();
            ((NodeResolver<ElementTs, Direction>::Resolve(i)), ...);
        }

        static void Resolve()
        {
            TagResolver<Tag>::Resolve();
            ((NodeResolver<ElementTs, Direction>::Resolve()), ...);
        }
    };
    // UNIT

    template <typename Graph, ResolverDirection Direction = ResolverDirection::RootFirst>
    struct ResolveOrder;

    template <typename Tag, ElementType... ElementTs, ResolverDirection Direction>
    struct ResolveOrder<Graph<Tag, ElementTs...>, Direction>
    {
        static void Resolve(int indent)
        {
            PrintResolverDirection<Direction>();
            NodeResolver<Graph<Tag, ElementTs...>, Direction>::Resolve(0);
        }

        static void Resolve()
        {
            PrintResolverDirection<Direction>();
            NodeResolver<Graph<Tag, ElementTs...>, Direction>::Resolve();
            // ((NodeResolver<ElementTs, Direction>::Resolve(0)), ...);// The template deployment is too fast. -> Pass to a graph-specific component before deployment.
            // ================================ Direction == ResolverDirection::RootFirst ================================
            // ResolverDirection::RootFirst
            // NodeResolver<Root>::Resolve();
            // NodeResolver<Child1>::Resolve();
            // NodeResolver<Child2>::Resolve();
            // NodeResolver<GrandChild1>::Resolve();
            // ...

            // ================================ Direction == ResolverDirection::LeafFirst ================================
            // ...
            // NodeResolver<GrandChild1>::Resolve();
            // NodeResolver<Child2>::Resolve();
            // NodeResolver<Child1>::Resolve();
            // NodeResolver<Root>::Resolve();

            // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ old ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
            // if constexpr (Direction == ResolverDirection::RootFirst)// This branching point may not be necessary
            // {
            //     std::cerr << "RootFirst\n";//
            //     {
            //     }
            // }
            // else
            // {
            //     std::cerr << "LeafFirst\n";
            //     {
            //         // pack
            //         // Here, it might be a good idea to pack the items back into the pack (stack) in order to reverse the order
            //         // NodeResolver<Tag, ResolverDirection::LeafFirst>::Resolve();
            //         ((NodeResolver<ElementTs, ResolverDirection::LeafFirst>::Resolve()), ...);
            //     }
            // }
            // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
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