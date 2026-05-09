#include <iostream>

#include "ComponentHierarchyDescription.h"
#include "HierarchyConstructor.h"

// build compile time dag

int main()
{
    using namespace tsr;

    std::cerr << "\n====== Ontology ========\n";
    ResolveOrder<Ontology_PysGraph>::Resolve();

    std::cerr << "\n====== Task Dependency ========\n";
    ResolveOrder<TaskDepndency_PysGraph>::Resolve();


    // 
    std::cerr << "\n====== Resolve Dependencies in the specified order ======\n";

    std::cerr << "======= Root First ======= \n";
    ResolveOrder<Ontology_PysGraph, ResolverDirection::RootFirst>::Resolve();

    
    std::cerr << "======= Leaf First ======= \n";
    ResolveOrder<Ontology_PysGraph, ResolverDirection::LeafFirst>::Resolve();

    // Type Force Checker
    // using ForceCheck = typename Ontology_PysGraph::element_types;
    return 0;
}

// ### 0.
// - Resolver<FooGraph::Bar>-> 