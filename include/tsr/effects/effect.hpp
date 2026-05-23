/*
 Descriptors for building an access effect system
*/

#pragma once


namespace tsr
{
    template<typename... Ts>
    struct ResourcePack{};

    template<typename T>
    struct Effects
    {
        using reads = ResourcePack<>;
        using writes = ResourcePack<>;
    };

    template<typename... TaskTs>
    struct ExecutionSet
    {};
}// namespace tsr