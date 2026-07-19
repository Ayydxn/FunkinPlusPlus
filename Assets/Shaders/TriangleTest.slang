struct VertexOutput
{
    float4 Position : SV_Position;
    float3 Color;
};

[shader("vertex")]
VertexOutput VertexMain(uint VertexID : SV_VertexID)
{
    static const float2 Positions[3] =
    {
        float2( 0.0, -0.5),
        float2( 0.5,  0.5),
        float2(-0.5,  0.5),
    };

    static const float3 Colors[3] =
    {
        float3(1.0, 0.0, 0.0),
        float3(0.0, 1.0, 0.0),
        float3(0.0, 0.0, 1.0),
    };

    VertexOutput Output;
    Output.Position = float4(Positions[VertexID], 0.0, 1.0);
    Output.Color = Colors[VertexID];

    return Output;
}

[shader("fragment")]
float4 FragmentMain(VertexOutput Input) : SV_Target
{
    return float4(Input.Color, 1.0);
}
