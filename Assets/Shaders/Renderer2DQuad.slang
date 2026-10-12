struct GlobalUniforms
{
    // World -> clip. Uploaded from a glm::mat4, which stores its columns one after another, so it's declared column_major here.
    // Slang's default matrix layout is row-major, without the keyword this would be read transposed.
    column_major float4x4 ViewProjectionMatrix;
};

struct VertexInput
{
    [vk::location(0)] float3 Position : POSITION;  // Already in world space, the CPU applied the quad's transform.
    [vk::location(1)] float4 Color : COLOR;
}

struct VertexOutput
{
    float4 Position : SV_Position;
    float4 Color;
};

[[vk::binding(0, 0)]] ConstantBuffer<GlobalUniforms> u_GlobalUniforms;

[shader("vertex")]
VertexOutput VertexMain(VertexInput Input)
{
    VertexOutput Output;
    Output.Position = mul(u_GlobalUniforms.ViewProjectionMatrix, float4(Input.Position, 1.0));
    Output.Color = Input.Color;
    
    return Output;
}

[shader("fragment")]
float4 FragmentMain(VertexOutput Input) : SV_Target
{
    return Input.Color;
}
