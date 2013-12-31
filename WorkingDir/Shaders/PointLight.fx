//-----------------------------------------------------------------------------------------
// Input and Output Structures
//-----------------------------------------------------------------------------------------

//texture data
Texture2D tColorMap;
Texture2D tNormalMap;
Texture2D tDepthMap;

TextureCube cShadowCubeMap;

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

DepthStencilState NoColorWriteDepthStencil
{
    DepthEnable = true;
    DepthFunc = LESS;
    DepthWriteMask = All;
    StencilEnable = true;
	FrontFaceStencilFail = REPLACE;
	FrontFaceStencilDepthFail = KEEP;
	FrontFaceStencilPass = KEEP;
	FrontFaceStencilFunc = ALWAYS;
	BackFaceStencilFail = REPLACE;
	BackFaceStencilDepthFail = KEEP;
	BackFaceStencilPass = KEEP;
	BackFaceStencilFunc = ALWAYS;
};

DepthStencilState ColorWriteDepthStencil
{
    DepthEnable = true;
    DepthFunc = ALWAYS;
    DepthWriteMask = All;
    StencilEnable = true;
	FrontFaceStencilFail = KEEP;
	FrontFaceStencilDepthFail = KEEP;
	FrontFaceStencilPass = KEEP;
	FrontFaceStencilFunc = EQUAL;
	BackFaceStencilFail = KEEP;
	BackFaceStencilDepthFail = KEEP;
	BackFaceStencilPass = KEEP;
	BackFaceStencilFunc = EQUAL;
};

DepthStencilState zWriteStencilDS
{
    DepthEnable = false;
    DepthFunc = GREATER_EQUAL;
    DepthWriteMask = All;
    StencilEnable = true;
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

struct VSSceneIn
{
	float3 Position	: POSITION;
};

struct PSSceneIn
{
	float4 Position  : SV_Position;		// SV_Position is a (S)ystem (V)ariable that denotes transformed position
	float4 ScreenPosition : TEXCOORD;
};

float VSM_FILTER(TextureCube img, float3 tex, float fragDepth)
{
    float lit = 0.0f;

	float occluder = img.Sample(LinearClampSampler, tex).x;
	float overdark = 0.05f;
	lit = saturate(exp(overdark * (occluder - fragDepth))) *0.5f;
    
    return lit;
}

//-----------------------------------------------------------------------------------------
// Constant Buffers (where we store variables by frequency of update)
//-----------------------------------------------------------------------------------------
cbuffer cbEveryLight
{
	matrix mWorld;
	
	float3 gLightPos;
	
	float3 gColor;
	
	float gLightRadius;
	float gLightIntensity = 1.0f;
	
	bool castShadows = false;
};

cbuffer cbPerFrame
{
	float3 gCameraPos;
	matrix mView;
	matrix mProj;
	//inverse View Projection
	matrix mIVP;
};

float4 Phong(float3 Position, float3 normal,
				float specularIntensity, float specularPower)
{
	//surface to light vector
	float3 lightVector = gLightPos.xyz - Position.xyz;

	//compute attenuation base on distance - linear attenuation
	float attenuation = (1.0f - length(lightVector) / gLightRadius);
	//float attenuation = saturate(1.0f - dot(lightVector / gLightRadius, lightVector / gLightRadius));
	
	//normalize light vector
	lightVector = normalize(lightVector);

	//compute diffuse light
	float NdL = max(0, dot(normal, lightVector));
	float3 diffuseLight = NdL * gColor.rgb;

	//reflection vector
	float3 reflectionVector = normalize(reflect(-lightVector, normal));

	//camera to surface vector
	float3 directionToCamera = normalize(gCameraPos - Position);

	//compute specular light
	float specularLight = specularIntensity * pow(saturate(dot(reflectionVector, directionToCamera)), specularPower);
	
	//output the two lights
	float4 finalColor = attenuation * gLightIntensity * float4(diffuseLight.rgb, specularLight);
	
	return finalColor;
}

//Normal Decoding Function
float3 decode(float4 enc)
{
	return (2.0f * enc.xyz - 1.0f);
}
//-----------------------------------------------------------------------------------------
// VertexShader: VSScene
//-----------------------------------------------------------------------------------------
PSSceneIn VSScene(VSSceneIn input)
{
	PSSceneIn output = (PSSceneIn)0;
	
	float4 worldPos = mul(float4(input.Position, 1.0f), mWorld);
	float4 viewPos = mul(worldPos, mView);

	output.Position = mul(viewPos, mProj);

	//Align texture coordinates
	output.ScreenPosition = output.Position;

	return output;
}

//-----------------------------------------------------------------------------------------
// PixelShader: PSSceneMain
//-----------------------------------------------------------------------------------------
float4 PSScene(PSSceneIn input) : SV_Target
{	
	//obtain screen position
	input.ScreenPosition.xy /= input.ScreenPosition.w;

	//obtain textureCoordinates corresponding to the current pixel
	//the screen coordinates are in [-1,1],[1,-1]
	//the texture coordinates need to be in [0,1],[0,1]

	float2 texCoord = 0.5f * (float2(input.ScreenPosition.x, -input.ScreenPosition.y) + 1.0f);

	//get normal data from the normalMap
	float4 normalData = tNormalMap.Sample(PointClampSampler, texCoord);

	//transform normal back into [-1,1] range
	float3 normal = decode(normalData);
	normal = normalize(normal);

	//get specular power, and get it into [0,255] range
	float specularPower = normalData.a * 255;

	//get specularIntensity from colorMap
	float specularIntensity = tColorMap.Sample(LinearClampSampler, texCoord).a;

	//read depth
	float depthVal = tDepthMap.Sample(PointClampSampler, texCoord).r;
	clip(-depthVal + 0.99999f);
	//compute screen-space position
	float4 position;

	position.xy = input.ScreenPosition.xy;
	position.z = depthVal;
	position.w = 1.0f;

	//transform to world space
	float4 worldPosition = mul(position, mIVP);
	worldPosition /= worldPosition.w;

	float ShadowFactor = 1.0f;
	if(castShadows)
	{
		float3 lightToPixel = worldPosition.xyz - gLightPos.xyz;
		float pixelDepth = length(lightToPixel);
		ShadowFactor = VSM_FILTER(cShadowCubeMap, lightToPixel, pixelDepth);
	}
	
	return ShadowFactor * Phong(worldPosition.xyz, normal, specularIntensity, specularPower);
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