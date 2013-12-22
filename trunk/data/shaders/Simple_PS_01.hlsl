
Texture2D shaderTexture;
SamplerState SampleType;

struct PixelShaderInput
{
    float4 pos : SV_POSITION;
    float3 color : COLOR0;
	float2 tex : TEXCOORD0;
};

float4 main(PixelShaderInput input) : SV_TARGET
{
    float4 texColorOut;
    // Sample the pixel color from the texture using the sampler at this texture coordinate location.
    texColorOut = shaderTexture.Sample(SampleType, input.tex);
	texColorOut.z = input.color.z;
    return texColorOut;
}
