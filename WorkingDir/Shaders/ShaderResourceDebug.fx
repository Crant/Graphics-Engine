Texture2D tTextureMap;

cbuffer cbPerRender
{
	bool bGrayScale = false;
};

SamplerState PointClampSampler
{
	Filter = MIN_MAG_MIP_POINT;
	AddressU = Clamp;
	AddressV = Clamp;
};

RasterizerState NoCulling
{
	CullMode = None;
};

BlendState NoBlend
{
	BlendEnable[0] = FALSE;
};

struct VS_IN
{
	float3 Position : POSITION;
	float2 TexCoord : TEXCOORD;
};

struct PS_IN
{
	float4 Position : SV_Position;
	float2 TexCoord : TEXCOORD;
};

PS_IN VS(VS_IN input)
{
	PS_IN output;

	output.Position = float4(input.Position, 1.0f);
	output.TexCoord = input.TexCoord;

	return output;
}

float4 PS(PS_IN input) : SV_Target
{
	float4 Color = tTextureMap.Sample(PointClampSampler, input.TexCoord);

	if(bGrayScale)
		Color = float4(Color.x, Color.x, Color.x, 1.0f);

	return Color;
}

technique11 Technique1
{
    pass P0
    {
        SetVertexShader( CompileShader( vs_4_0, VS() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, PS() ) );

		SetRasterizerState( NoCulling );
		SetBlendState(NoBlend, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
    }
}