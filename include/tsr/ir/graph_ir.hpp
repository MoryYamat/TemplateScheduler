#pragma once

namespace tsr
{
    // ir/graph_ir.hpp
    template <typename Tag, typename NodePackT, typename RelationPackT>
    struct GraphIR
    {
        using tag_type = Tag;
        using nodes = NodePackT;
        using relations = RelationPackT;
    };

    // template<typename Tag, typename NodePackT, typename RelationPackT>
    // struct GraphIR
    // {
    //     using tag_type = Tag;
    //     using nodes = NodePackT;
    //     using relations = RelationPackT;
    // };

} // namespace tsr