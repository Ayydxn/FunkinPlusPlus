#pragma once

#include "Core/CoreTypes.h"
#include "RHICore/VertexBuffer.h"

#include <glm/glm.hpp>

#include <cstddef>

// One corner of a quad, already in world space. CRenderer2D writes four of these per quad.
struct FQuadVertex
{
    glm::vec3 Position;
    glm::vec4 Color;
};

static_assert(sizeof(FQuadVertex) == 28, "FQuadVertex is 28 bytes: 12 (position) + 16 (color). It's copied straight into a vertex buffer, so don't enable GLM_FORCE_DEFAULT_ALIGNED_GENTYPES.");
static_assert(offsetof(FQuadVertex, Position) == 0);
static_assert(offsetof(FQuadVertex, Color) == 12);

// What the vertex shader reads from the uniform buffer. Must match GlobalUniforms in Renderer2DQuad.slang (a float4x4, std140).
struct FGlobalUniforms
{
    glm::mat4 ViewProjectionMatrix;
};

static_assert(sizeof(FGlobalUniforms) == 64, "FGlobalUniforms must match GlobalUniforms in Renderer2DQuad.slang (a float4x4).");

// The buffer vertex layout of FQuadVertex. This is the one place it's defined, both the pipeline and the vertex buffers get it from here.
inline FVertexBufferLayout GetQuadVertexBufferLayout()
{
    return {
        { "Position", EShaderDataType::Float3 },
        { "Color", EShaderDataType::Float4 }
    };
}

// Where a quad's four corners sit before its transform is applied: the unit square, with the origin at the top-left and +Y pointing down.
// A quad's transform maps this square onto the quad's place in the world, so a sprite's position is where its top-left corner ends up.
// The order is top-left, top-right, bottom-right, bottom-left, which is what the index buffer in CRenderer2D expects.
inline constexpr glm::vec2 QuadCorners[4] =
{
    { 0.0f, 0.0f },
    { 1.0f, 0.0f },
    { 1.0f, 1.0f },
    { 0.0f, 1.0f }
};
