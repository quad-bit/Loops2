#pragma once

#include <functional>
#include "Utility/RenderingWrappers/RenderingWrapper.h"

//template <typename T, typename... Rest>
//inline void HashCombine(std::size_t &seed, T const &v, Rest &&... rest) {
//    std::hash<T> hasher;
//    seed ^= hasher(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
//    (int[]) {  0, (HashCombine(seed, std::forward<Rest>(rest)), 0)... };
//}

inline void HashCombine(std::size_t& seed) { }

template <typename T, typename... Rest>
inline void HashCombine(std::size_t& seed, const T& v, Rest... rest) 
{
    std::hash<T> hasher;
    seed ^= hasher(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    HashCombine(seed, rest...);
}

bool operator==(const VertexInputAttributeInfo & lhs, const VertexInputAttributeInfo & rhs)
{
    if (lhs.binding == rhs.binding && lhs.format == rhs.format &&
        lhs.location == rhs.location && lhs.offset == rhs.offset)
        return true;
    else
        return false;
}

bool operator==(const VertexInputBindingInfo & lhs, const VertexInputBindingInfo & rhs)
{
    if (lhs.binding == rhs.binding && lhs.inputRate == rhs.inputRate &&
        lhs.stride == rhs.stride)
        return true;
    else
        return false;
}

bool operator==(const SetWrapper & lhs, const SetWrapper & rhs)
{
    if (lhs.setValue == rhs.setValue && lhs.bindingWrapperList.size() == rhs.bindingWrapperList.size())
        return true;
    else
        return false;
}


bool operator==(const StencilOpState & lhs, const StencilOpState & rhs)
{
    if (lhs.compareMask == rhs.compareMask && 
        lhs.writeMask == rhs.writeMask &&
        lhs.reference == rhs.reference &&
        lhs.passOp == rhs.passOp &&
        lhs.failOp == rhs.failOp &&
        lhs.depthFailOp == rhs.depthFailOp
        )
        return true;
    else
        return false;
}


namespace std
{
    template<> struct hash<VertexInputAttributeInfo>
    {
        std::size_t operator()(VertexInputAttributeInfo const& s) const noexcept
        {
            std::size_t h1 = std::hash<std::uint32_t>{}(s.binding);
            std::size_t h2 = std::hash<std::uint32_t>{}(s.location);
            std::size_t h3 = std::hash<std::uint32_t>{}(s.offset);
            std::size_t h4 = std::hash<std::uint32_t>{}((uint32_t)s.format);

            std::size_t seed = 0UL;
            HashCombine(seed, h1, h2, h3, h4);

            return seed; // or use boost::hash_combine
        }
    };

    template<> struct hash<VertexInputBindingInfo>
    {
        std::size_t operator()(VertexInputBindingInfo const& s) const noexcept
        {
            std::size_t h1 = std::hash<std::uint32_t>{}(s.binding);
            std::size_t h2 = std::hash<std::uint32_t>{}(s.stride);
            std::size_t h3 = std::hash<std::uint32_t>{}((uint32_t)s.inputRate);

            std::size_t seed = 0UL;
            HashCombine(seed, h1, h2, h3);

            return seed; // or use boost::hash_combine
        }
    };

    template<> struct hash<StencilOpState>
    {
        std::size_t operator()(StencilOpState const& s) const noexcept
        {
            std::size_t h1 = std::hash<std::uint32_t>{}((uint32_t)s.compareOp);
            std::size_t h2 = std::hash<std::uint32_t>{}((uint32_t)s.depthFailOp);
            std::size_t h3 = std::hash<std::uint32_t>{}((uint32_t)s.failOp);
            std::size_t h4 = std::hash<std::uint32_t>{}((uint32_t)s.passOp);
            std::size_t h5 = std::hash<std::uint32_t>{}(s.reference);
            std::size_t h6 = std::hash<std::uint32_t>{}(s.writeMask);
            std::size_t h7 = std::hash<std::uint32_t>{}(s.compareMask);

            std::size_t seed = 0UL;
            HashCombine(seed, h1, h2, h3, h4, h5, h6, h7);

            return seed; // or use boost::hash_combine
        }
    };

    template<> struct hash<SetWrapper>
    {
        std::size_t operator()(SetWrapper const& s) const noexcept
        {
            std::size_t seed = 0UL;
            std::size_t h1 = std::hash<std::uint32_t>{}(s.setValue);

            std::size_t bindingHash = 0UL;

            for (uint32_t i = 0; i < s.bindingWrapperList.size(); i++)
            {
                BindingWrapper bindingWrapper = s.bindingWrapperList[i];
                std::size_t h1 = std::hash<std::uint32_t>{}(bindingWrapper.bindingObj.binding);
                std::size_t h2 = std::hash<std::uint32_t>{}(bindingWrapper.bindingObj.descriptorCount);
                std::size_t h3 = std::hash<std::uint32_t>{}((std::uint32_t)bindingWrapper.bindingObj.descriptorType);
                
                std::size_t memberHash = 0UL;
                for each(auto member in bindingWrapper.memberList)
                {
                    std::size_t h1 = std::hash<std::uint32_t>{}(member.offset);
                    std::size_t h2 = std::hash<std::uint32_t>{}(member.size);

                    std::size_t hash = 0UL;
                    HashCombine(hash, h1, h2);

                    memberHash += hash;
                }

                HashCombine(bindingHash, memberHash, h1, h2, h3);
            }

            HashCombine(seed, bindingHash, h1);

            return seed; // or use boost::hash_combine
        }
    };
}

