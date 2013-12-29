//--------------------------------------------------------------------------------------
// Particle.fx
// Direct3D 10 Shader Model 4.0 Line Drawing Demo
// Copyright (c) Stefan Petersson, 2008
//--------------------------------------------------------------------------------------

//-----------------------------------------------------------------------------------------
// Input and Output Structures
//-----------------------------------------------------------------------------------------

BlendState AdditiveBlending
{
    AlphaToCoverageEnable = FALSE;
    BlendEnable[0] = TRUE;
    SrcBlend = ONE;
    DestBlend = ONE;
    BlendOp = ADD;
    SrcBlendAlpha = ONE;
    DestBlendAlpha = ONE;
    BlendOpAlpha = ADD;
    RenderTargetWriteMask[0] = 0x0F;
};

BlendState ColorWriteDisable
{
	BlendEnable[0] = FALSE;
	SrcBlend = ZERO;
    DestBlend = ZERO;
	RenderTargetWriteMask[0] = 0x0F;
};

RasterizerState frontCulling
{
    CullMode = Front;
	FillMode = Solid;
};

SamplerState LinearClampSampler
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Clamp;
    AddressV = Clamp;
};

SamplerState PointClampSampler
{
    Filter = MIN_MAG_MIP_POINT;
    AddressU = Clamp;
    AddressV = Clamp;
};

//texture data
Texture2D tColorMap;
Texture2D tNormalMap;
Texture2D tDepthMap;

Texture2D tShadowMap;
Texture2D tCookie;
struct VSSceneIn
{
	float3 Pos	: POSITION;
};

struct PSSceneIn
{
	float4 Pos  : SV_Position;		// SV_Position is a (S)ystem (V)ariable that denotes transformed position
	float4 ScreenPosition : TEXCOORD1;
};
//-----------------------------------------------------------------------------------------
// Constant Buffers (where we store variables by frequency of update)
//-----------------------------------------------------------------------------------------
cbuffer cbEveryFrame
{
	matrix mWorld;
	matrix mView;
	matrix mProj;
	//inverse View Projection
	matrix mIVP;
	matrix mLightViewProjection;

	float gLightAngle;
	float gLightHeight;
	float gLightIntensity;
	float3 gLightPos;
	float3 gLightDir;
	float3 gLightColor;
	float3 gCameraPos;
	bool castShadow = false;
	float SMAP_SIZE = 2048;
};
cbuffer cbRarely
{
	float2 halfPixel;
};

//Normal Decoding Function
float3 decode(float4 enc)
{
	return (2.0f * enc.xyz - 1.0f);
}

float VSM_FILTER(Texture2D img, float2 tex, float fragDepth)
{
    float lit = 0.0f;

	float occluder = img.Sample(LinearClampSampler, tex).x;
	float overdark = 0.05f;
	lit = saturate(exp(overdark * (occluder - fragDepth))) *0.5f;
    
    return lit;
}

//-----------------------------------------------------------------------------------------
// VertexShader: VSScene
//-----------------------------------------------------------------------------------------
PSSceneIn VSScene(VSSceneIn input)
{
	PSSceneIn output = (PSSceneIn)0;
	float4 worldPos = mul(float4(input.Pos, 1.0f), mWorld);
	float4 viewPos = mul(worldPos, mView);
	output.Pos = mul(viewPos, mProj);

	//Align texture coordinates
	output.ScreenPosition = output.Pos;

	return output;
}

//-----------------------------------------------------------------------------------------
// PixelShader: PSSceneMain
//-----------------------------------------------------------------------------------------
float4 Phong(float3 Position, float3 Normal, float radialAttenuation,
				float SpecularIntensity, float SpecularPower)
{
	float3 LightVector = gLightPos - Position;

	//Calculate Attenuation
	float heightAttenuation = 1.0f - saturate(length(LightVector) - (gLightHeight/2));

	//heightAttenuation = saturate(1.0f - length(LightVector)/(gLightHeight/2));
	float Attenuation = min(radialAttenuation, heightAttenuation);

	LightVector = normalize(LightVector);
	
	float SdL = dot(LightVector, gLightDir);

	float4 finalColor = 0;
	if(SdL <= gLightAngle)
	{
		//Calculate Reflection Vector
		float3 reflectionVector = normalize(reflect(-LightVector, Normal));

		//Calculate Eye vector
		float3 directionToCamera = normalize(gCameraPos - Position);

		//Calculate Normal dot LightVector
		float NdL = max(0, dot(Normal, LightVector));

		//Calculate Diffuse color
		float3 diffuseLight = NdL * gLightColor.rgb;
		
		//compute specular light
		float specularLight = SpecularIntensity * pow(saturate(dot(reflectionVector, directionToCamera)), SpecularPower);
	
		finalColor = Attenuation * gLightIntensity * float4(diffuseLight.rgb, specularLight);
	}
	return finalColor;
}
float4 PSScene(PSSceneIn input) : SV_Target
{	
	
	//obtain screen position
	input.ScreenPosition.xy /= input.ScreenPosition.w;

	//obtain textureCoordinates corresponding to the current pixel
	//the screen coordinates are in [-1,1],[1,-1]
	//the texture coordinates need to be in [0,1],[0,1]

	float2 texCoord = 0.5f * float2(input.ScreenPosition.x, -input.ScreenPosition.y) + 0.5f;

	//align texels to pixels
	texCoord -= halfPixel;

	//get normal data from the normalMap
	float4 normalData = tNormalMap.Sample(PointClampSampler, texCoord);

	//transform normal back into [-1,1] range
	float3 normal = normalize(decode(normalData));

	//get specular power, and get it into [0,255] range
	float specularPower = normalData.a * 255;

	//get specularIntensity from colorMap
	float specularIntensity = tColorMap.Sample(LinearClampSampler, texCoord).a;

	//read depth
	float depthVal = tDepthMap.Sample(PointClampSampler, texCoord).r;
	clip(-depthVal + 0.9999f);
	//compute screen-space position
	float4 position;

	position.xy = input.ScreenPosition.xy;
	position.z = depthVal;
	position.w = 1.0f;

	//transform to world space
	float4 worldPosition = mul(position, mIVP);
	worldPosition /= worldPosition.w;

	//Calculate shadows
	float ShadowFactor = 1.0f;

	float4 lightPos = mul(worldPosition, mLightViewProjection);

	lightPos.xy /= lightPos.w;

	//Calculate TexCoords from light Point of view
	float2 LightTexCoord = 0.5f * float2(lightPos.x, -lightPos.y) + 0.5f;
		
	float Attenuation = tCookie.Sample(LinearClampSampler, LightTexCoord).r;
	if(castShadow)
	{
		float lightDepth = length(worldPosition.xyz - gLightPos);

		ShadowFactor = VSM_FILTER(tShadowMap, LightTexCoord, lightDepth);
	}
	return ShadowFactor * Phong(worldPosition.xyz, normal, Attenuation, specularIntensity, specularPower);
}

float4 Empty_PSScene(PSSceneIn input) : SV_Target
{
	return float4(0,0,0,0);
}

//-----------------------------------------------------------------------------------------
// Technique: RenderTextured  
//-----------------------------------------------------------------------------------------
technique11 Technique1
{  
	pass p0
	{
		// Set VS, GS, and PS
        SetVertexShader( CompileShader( vs_4_0, VSScene() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, Empty_PSScene() ) );
		
		SetRasterizerState(frontCulling);
		SetDepthStencilState( NoColorWriteDepthStencil, 0 );
	    //SetDepthStencilState( zWriteNoStencilEnable, 0 );
		SetBlendState(ColorWriteDisable, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
	}

	pass p1
    {
		// Set VS, GS, and PS
        SetVertexShader( CompileShader( vs_4_0, VSScene() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, PSScene() ) );
		
		SetRasterizerState(frontCulling);
		SetDepthStencilState( ColorWriteDepthStencil, 0 );
	    //SetDepthStencilState( zWriteNoStencilEnable, 0 );
		SetBlendState(AdditiveBlending, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
    }
}
