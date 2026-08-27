#pragma once

#include "Core/CoreTypes.h"

enum class EShaderStage : uint32
{
    None     = 0,
    
    Vertex   = 1 << 0,
    Fragment = 1 << 4,
    Compute  = 1 << 5
};

constexpr EShaderStage operator|(EShaderStage Left, EShaderStage Right)
{
    return static_cast<EShaderStage>(static_cast<uint32>(Left) | static_cast<uint32>(Right));
}

constexpr EShaderStage operator&(EShaderStage Left, EShaderStage Right)
{
    return static_cast<EShaderStage>(static_cast<uint32>(Left) & static_cast<uint32>(Right));
}

constexpr EShaderStage& operator|=(EShaderStage& Left, EShaderStage Right)
{
    Left = Left | Right;
    return Left;
}

constexpr bool HasShaderStageFlag(EShaderStage ShaderStageFlags, EShaderStage Test)
{
    return (ShaderStageFlags & Test) == Test;
}
