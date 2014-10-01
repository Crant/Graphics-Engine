//--------------------------------------------------------------------------------------
// Particle.fx
// Direct3D 10 Shader Model 4.0 Line Drawing Demo
// Copyright (c) Stefan Petersson, 2008
//--------------------------------------------------------------------------------------

//-----------------------------------------------------------------------------------------
// Input and Output Structures
//-----------------------------------------------------------------------------------------



//texture data
Texture2D tDepthMap;
Texture2D tFinalScene;
Texture2D tRefractionMap;

Texture2D tNormalMap;

RasterizerState backCulling
{
    CullMode = Back;
	FillMode = Solid;
};

DepthStencilState zWriteStencilDS
{
    DepthEnable = true;
    DepthFunc = Less_Equal;
    DepthWriteMask = All;
    StencilEnable = true;
};

BlendState NoBlend
{
    RenderTargetWriteMask[0] = 0x0f;
    BlendEnable[0] = false;
};

SamplerState LinearWrapSampler
{
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = Wrap;
	AddressV = Wrap;
};

SamplerState PointClampSampler
{
	Filter = MIN_MAG_MIP_POINT;
	AddressU = Clamp;
	AddressV = Clamp;
};

struct VSSceneIn
{
	float3 Position	: POSITION;
	float2 TexCoord : TEXCOORD;
};

struct PSSceneIn
{
	float4 Position  : SV_Position;		// SV_Position is a (S)ystem (V)ariable that denotes transformed position
	float2 TexCoord : TEXCOORD;
};

//Normal Decoding Function
float3 decode(float4 enc)
{
	return (2.0f * enc.xyz - 1.0f);
}

//-----------------------------------------------------------------------------------------
// Constant Buffers (where we store variables by frequency of update)
//-----------------------------------------------------------------------------------------
cbuffer cbEveryFrame
{
	//inverse View Projection
	matrix mInverseViewProj;
	float3 gCameraPos;
	float2 halfPixel;
	float  refractScale = 0.01f;
	float  waterTranslation = 0.1f;
	float  WaterLevel = 80;
};
//-----------------------------------------------------------------------------------------
// VertexShader: VSScene
//-----------------------------------------------------------------------------------------
PSSceneIn VSScene(VSSceneIn input)
{
	PSSceneIn output = (PSSceneIn)0;
	
	output.Position = float4(input.Position, 1.0f);
	output.TexCoord = input.TexCoord;
	return output;
}

//-----------------------------------------------------------------------------------------
// PixelShader: PSSceneMain
//-----------------------------------------------------------------------------------------
float4 PSScene(PSSceneIn input) : SV_Target
{	
	float depthVal = tDepthMap.Sample(PointClampSampler, input.TexCoord).r;
	
	//Compute Waves
	float2 bumpMapTexCoord = input.TexCoord;
	bumpMapTexCoord.y += waterTranslation;

	//compute screen-space position
	float4 position;
	position.x = input.TexCoord.x * 2.0f - 1.0f;
	position.y = -(input.TexCoord.y * 2.0f -1.0f);
	position.z = depthVal;
	position.w = 1.0f;

	//transform to world space
	float4 worldPosition = mul(position, mInverseViewProj);
	worldPosition.xyz /= worldPosition.w;

	//Calculate Refraction texture coordinates
	float4 refractionPosition = position;

	//float3 eyeVector = normalize(gCameraPos - input.Position3D.xyz);

	float2 refractTexCoord;
	refractTexCoord.x = 0.5f * (refractionPosition.x / refractionPosition.w) + 0.5f;
	refractTexCoord.y = 0.5f * (-refractionPosition.y / refractionPosition.w) + 0.5f;

	float4 normalData = tNormalMap.Sample(LinearWrapSampler, bumpMapTexCoord);

	float3 normal = decode(normalData);

	refractTexCoord = (refractTexCoord + (normal.xy * refractScale)) - halfPixel;;

	float4 refractionColor = tRefractionMap.Sample(PointClampSampler, refractTexCoord);

	float4 FinalColor;
	if(refractionColor.x == 0.0f && refractionColor.y == 0.0f && refractionColor.z == 0.0f)
	{
		float4 FinalScene = tFinalScene.Sample(PointClampSampler, input.TexCoord);
		float4 SceneColor = float4(FinalScene.xyz, 1.0f);
		FinalColor = SceneColor;
	}
	else
	{
		float4 dullColor = float4(0.3f, 0.3f, 0.5f, 1.0f);
		refractionColor = float4(refractionColor.xyz, 1.0f);
		//float r = (1.2f - 1.0f) / (1.2f + 1.0f);
		//float fresnelFactor = max(0.0f, min(1.0f, r + (1.0f - r) * pow(1.0f - dot
		float lerpValue = 0.3f;
		float4 WaterColor = lerp(refractionColor, dullColor, lerpValue);
		FinalColor = WaterColor;
	}
	return FinalColor;
}
//-----------------------------------------------------------------------------------------
// Technique: RenderTextured  
//-----------------------------------------------------------------------------------------
technique10 Technique1
{  
	pass p0
    {
		// Set VS, GS, and PS
        SetVertexShader( CompileShader( vs_4_0, VSScene() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, PSScene() ) );
		//
		SetRasterizerState( backCulling );
//
	    SetDepthStencilState( zWriteStencilDS, 0 );
		SetBlendState(NoBlend, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
//
    }
}
