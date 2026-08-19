struct VertexInput
{
    [vk::location(0)] float3 Position : POSITION;
}

struct VertexOutput
{
    float4 Position : SV_Position;
    float3 Color;
};

[shader("vertex")]
VertexOutput VertexMain(VertexInput Input)
{
    VertexOutput Output;
    Output.Position = float4(Input.Position, 1.0);
    Output.Color = normalize(abs(Input.Position)) * 0.5 + 0.5;

    return Output;
}

[shader("fragment")]
float4 FragmentMain(VertexOutput Input) : SV_Target
{
    return float4(Input.Color, 1.0);
}
