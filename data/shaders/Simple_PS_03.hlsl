
Texture2D shaderTexture;
SamplerState SampleType;

cbuffer ColorConstantBuffer
{
	float4 saturation;
	float4 luminance;
};

struct PixelShaderInput
{
    float4 pos : SV_POSITION;
    float3 normal : NORMAL;
	float2 tex : TEXCOORD0;		
};

float4 main(PixelShaderInput input) : SV_TARGET
{
    float4 texColorOut;
    // Sample the pixel color from the texture using the sampler at this texture coordinate location.
    texColorOut = shaderTexture.Sample(SampleType, input.tex);
	//float3 luminanceVal = dot( texColorOut, float3(luminance.x,luminance.y,luminance.z));
	//float4 finalCol         = float4( lerp( luminanceVal , texColorOut, saturation ),1.0f);
	//finalCol.z = texColorOut.z;
    //return finalCol;
	return texColorOut;
}
