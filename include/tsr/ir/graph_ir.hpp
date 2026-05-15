#pragma once

namespace tsr
{
    // ir/graph_ir.hpp
    template <typename Tag, typename RelationPackT>
    struct GraphIR
    {
        using tag_type = Tag;
        using relations = RelationPackT;
    };
} // namespace tsr