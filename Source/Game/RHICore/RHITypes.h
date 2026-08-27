#pragma once

#include "Core/CoreTypes.h"

enum class EPrimitiveTopology : uint8
{
    Points,
    Lines,
    LineStrip,
    Triangles,
    TriangleStrip,
    TriangleFan
};

enum class ECullMode : uint8
{
    None,
    Front,
    Back,
    FrontAndBack
};

enum class EFillMode : uint8
{
    Solid,
    Wireframe
};

enum class EFrontFace : uint8
{
    Clockwise,
    CounterClockwise
};

enum class ECompareOperation : uint8
{
    Never,
    Less,
    Equal,
    LessOrEqual,
    Greater,
    NotEqual,
    GreaterOrEqual,
    Always
};

enum class EBlendFactor : uint8
{
    Zero,
    One,
    SrcColor,
    OneMinusSrcColor,
    SrcAlpha,
    OneMinusSrcAlpha,
    DstColor,
    OneMinusDstColor,
    DstAlpha,
    OneMinusDstAlpha
};

enum class EBlendOperation : uint8
{
    Add,
    Subtract,
    ReverseSubtract,
    Min,
    Max
};
