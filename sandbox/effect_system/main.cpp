#include <iostream>
#include <type_traits>

#include "tsr/effects/effect.hpp"// dsl
#include "tsr/effects/plan.hpp"// compiler 

#include "dsl_descriptions.hpp"// Required for extraction
#include "dsl_effects.hpp"// Required for extraction


int main()
{
    std::cout << "hello effect system\n";

    using namespace tsr;
    using namespace es;

    using CES_EXE_SET = ExecutionSet<CES_PROCESS_N_INTEG,CES_PROCESS_N_COLL,CES_PROCESS_N_RENDER>;
    using Expected_CES_EXE_SET = ExecutionSet<Node<Integrate>, Node<CollisionDetection>,Node<Renderer>>;
    static_assert(std::is_same_v<Expected_CES_EXE_SET, CES_EXE_SET>, "Error in `Execution Set`");

    // extractors
    // reads
    using CES_EXTRACT_Reads_FROM_Effects_INTEG = typename ExtractReads<Effects<Integrate>>::type;
    using Expected_CES_EXTRACT_Reads_INTEG = ResourcePack<es::Velocity, es::Acceleration>;
    using CES_EXTRACT_Reads_FROM_Node_INTEG = typename ExtractReads<Node<Integrate>>::type;
    static_assert(std::is_same_v<Expected_CES_EXTRACT_Reads_INTEG, CES_EXTRACT_Reads_FROM_Effects_INTEG>, "Error in `ExtractReads`");
    static_assert(std::is_same_v<Expected_CES_EXTRACT_Reads_INTEG, CES_EXTRACT_Reads_FROM_Node_INTEG>, "Error in `ExtractReads");
    // std::cerr << "Extract From Effects = " << typeid(CES_EXTRACT_Reads_FROM_Effects_INTEG).name() << "\n";
    // std::cerr << "Extract From Node = " << typeid(CES_EXTRACT_Reads_FROM_Node_INTEG).name() << "\n";
    // writes
    using CES_EXTRACT_Writes_FROM_Effects_INTEG = typename ExtractWrites<Effects<Integrate>>::type;
    using CES_EXTRACT_Writes_FROM_Node_INTEG = typename ExtractWrites<Node<Integrate>>::type;
    using Expected_CES_EXTRACT_Writes_INTEG = ResourcePack<es::Position>;
    static_assert(std::is_same_v<Expected_CES_EXTRACT_Writes_INTEG, CES_EXTRACT_Writes_FROM_Effects_INTEG>, "Error in `ExtractWrites`");
    static_assert(std::is_same_v<Expected_CES_EXTRACT_Writes_INTEG, CES_EXTRACT_Writes_FROM_Node_INTEG>, "Error in `ExtractWrites");

    using CES_HAS_RESOURCE_INTERSECTION_INTEG = HasReadWriteConflict<Effects<Integrate>, Effects<CollisionDetection>>;      // Effects<Integrate>::reads && Effects<CollisionDetection>::writes -> Empty()
    static_assert(!CES_HAS_RESOURCE_INTERSECTION_INTEG::type::value, "Error in `DetermineReadWrite`");                      // expected == False

    // <Integrate>::writes::position and <CollisionDetection>::reads::position conflict
    static_assert(!CanRunTogether_v<CES_PROCESS_N_INTEG, CES_PROCESS_N_COLL>, "Error in `CanRunTogether" );                 // expected == False 
    // 
    // using Plan = typename MakeSafeLayeredPlan<ExecutionSet<Node<A>, Node<B>, Node<C>>>::type;
    

    return 0;
}