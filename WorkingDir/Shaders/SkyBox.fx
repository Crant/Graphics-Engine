cbuffer cbPerFrame
{
	matrix mWorldViewProjection;
	matrix mWorld;

	float FogHeight;
	float CameraY;
	float3 FogColor = float3(0.6f, 0.6f, 0.6f);
}

TextureCube gSkyMap;

SamplerState linearSampler 
{
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = Wrap;
	AddressV = Wrap;
};

BlendState SrcAlphaBlendingAdd 
{ 
	BlendEnable[0] = TRUE; 
	SrcBlend = SRC_ALPHA; 
	DestBlend = INV_SRC_ALPHA; 
	BlendOp = ADD; 
	SrcBlendAlpha = ZERO; 
	DestBlendAlpha = ZERO; 
	BlendOpAlpha = ADD; 
	RenderTargetWriteMask[0] = 0x0F; 
};

DepthStencilState EnableDepth
{
	DepthEnable = TRUE;
	DepthWriteMask = ZERO;
	DepthFunc = LESS_EQUAL;
};

RasterizerState NoCull
{
    CullMode = None;
};

struct VS_IN
{
	float3 Pos : POSITION;
};

struct VS_OUT
{
	float4 Pos : SV_Position;
    float3 worldPos : POSITION;
	float3 texCoord : TEXCOORD;
};

VS_OUT VSScene(VS_IN input)
{
	VS_OUT output = (VS_OUT)0;

	
	// set z = w so that z/w = 1 (i.e., skydome always on far plane).
	output.Pos = mul(float4(input.Pos, 1.0f), mWorldViewProjection).xyww;

	// use local vertex position as cubemap lookup vector.
	output.texCoord = input.Pos;
	output.worldPos = mul(float4(input.Pos, 1.0f), mWorld).xyz;

	return output;
}

float4 PSScene(VS_OUT input) : SV_Target
{
	float4 output = gSkyMap.Sample(linearSampler, input.texCoord);

	//if(input.worldPos.y < FogHeight)
	//{
	//	float fogFactor = (input.worldPos - CameraY) / (FogHeight - CameraY);

	//	output = lerp(float4(FogColor, 1.0f), output, saturate(fogFactor));
	//}
	return output;
}

technique11 SkyTechDeferred
{
    pass P0
    {
        SetVertexShader( CompileShader( vs_4_0, VSScene() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, PSScene() ) );
        
        SetDepthStencilState( EnableDepth, 0 );
	    SetRasterizerState( NoCull );
		SetBlendState( SrcAlphaBlendingAdd, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
    }
}