#include <iostream>

#include "ComponentHierarchyDescription.h"
#include "HierarchyConstructor.h"

// build compile time dag

int main()
{
    using namespace tsr;

    std::cerr << "\n====== Resolve Dependencies in the specified order ======\n";

    // ResolveOrder<Ontology_PysGraph, ResolverDirection::RootFirst>::Resolve();
    // ResolveOrder<Ontology_PysGraph, ResolverDirection::LeafFirst>::Resolve();

    NodeResolver<Ontology_PysGraph>::Resolve();

    std::cerr << "\n====== Task Dependency ========\n";
    NodeResolver<TaskDepndency_PysGraph>::Resolve();

    using ForceCheck = typename Ontology_PysGraph::element_types;
    return 0;
}

// ### 0.
// - Resolver<FooGraph::Bar>-> 