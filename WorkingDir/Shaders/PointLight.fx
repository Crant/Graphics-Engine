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

/* 
   - disable color writes
   - disable z-buffer writes
   - z-function = LESS
   - stencil enabled
   - stencilfunc = ALWAYS
   - stencilops = all KEEP but STENCILPASS = REPLACE
 */
DepthStencilState FrontFaceDepthStencil
{
    DepthEnable = false;
    DepthFunc = LESS;
    DepthWriteMask = zero;
    StencilEnable = true;
	FrontFaceStencilFail = KEEP;
	FrontFaceStencilDepthFail = KEEP;
	FrontFaceStencilPass = REPLACE;
	FrontFaceStencilFunc = ALWAYS;
	BackFaceStencilFail = KEEP;
	BackFaceStencilDepthFail = KEEP;
	BackFaceStencilPass = REPLACE;
	BackFaceStencilFunc = ALWAYS;
};

/* 
   - color writes enabled
   - z-buffer writes disabled
   - z-function = GREATEREQUAL
   - stenfilfunc = EQUAL
   - stencilops = all KEEP
 */
DepthStencilState BackFaceDepthStencil
{
    DepthEnable = false;
    DepthFunc = GREATER_EQUAL;
    DepthWriteMask = zero;
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

RasterizerState frontCulling
{
    CullMode = Front;
	FillMode = Solid;
};
RasterizerState backCulling
{
    CullMode = Back;
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
	float overdark = 10.0f;
	lit = exp(overdark * occluder) * exp(-overdark * fragDepth);
	lit = saturate(lit);
    
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
	float3 lightVector = gLightPos - Position;

	//compute attenuation base on distance - linear attenuation
	//float attenuation = saturate(1.0f - (length(lightVector) / gLightRadius));

	//float attenuation = saturate(1.0f - dot(lightVector / gLightRadius, lightVector / gLightRadius));

	float attenuation = 1.0f - saturate(length(lightVector) / gLightRadius);
	attenuation = pow(attenuation, 2);
	//normalize light vector
	float3 L = normalize(lightVector);

	//compute diffuse light
	float NdL = max(0.0f, dot(normal, L));
	float3 diffuseLight = NdL * gColor; // När NdL blir 0 så blir det "Skuggor" i terrängen

	//reflection vector
	float3 reflectionVector = normalize(reflect(-L, normal));

	//camera to surface vector
	float3 directionToCamera = normalize(gCameraPos - Position);

	//compute specular light
	float specularLight = specularIntensity * pow(saturate(dot(reflectionVector, directionToCamera)), specularPower);
	
	//output the two lights
	float4 finalColor = attenuation * gLightIntensity * float4(diffuseLight.rgb, specularLight);
	
	return finalColor;
}

float4 Phong2(float3 Position, float3 normal,
				float specularIntensity, float specularPower)
{
	float3 finalColor = float3(0.0f, 0.0f, 0.0f);

	//Create the vector between light position and pixels position
	float3 lightToPixelVec = gLightPos.xyz - Position.xyz;

	//Find the distance between the light pos and pixel pos
	float d = length(lightToPixelVec);

	//Create the ambient light
	float3 finalAmbient = gColor.rgb * float3(0.3f, 0.3f, 0.3f);

	//If pixel is too far, return pixel color with ambient light
	if(d > gLightRadius)
		return float4(finalAmbient, 1.0f);

	//Turn lightToPixelVec into a unit length vector describing
	//the pixels direction from the lights position
	lightToPixelVec /= d;

	//Calculate how much light the pixel gets by the angle
	//in which the light strikes the pixels surface
	float howMuchLight = dot(lightToPixelVec, normal);

	//If light is striking the front side of the pixel
	if(howMuchLight > 0.0f)
	{
		//Add light to the finalColor of the pixel
		finalColor += howMuchLight * gColor.rgb;
		
		//Calculate Light's falloff factor
		float3 attenuation = float3(1.0f, 0.2f, 0.0f);
		finalColor /=  attenuation.x + (attenuation.y * d) + (attenuation.z * (d*d));
	}

	//make sure the values are between 1 and 0, and add the ambient
	finalColor = saturate(finalColor + finalAmbient);

	return float4(finalColor, 1.0f);
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
	float3 normal = normalize(decode(normalData));

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
	//Pass 1
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

	//Pass 2
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

	//Inside Camera
	pass p3
    {
		// Set VS, GS, and PS
        SetVertexShader( CompileShader( vs_4_0, VSScene() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, PSScene() ) );
		
		SetRasterizerState(backCulling);
		SetDepthStencilState( FrontFaceDepthStencil, 0 );
	    //SetDepthStencilState( zWriteNoStencilEnable, 0 );
		SetBlendState(ColorWriteDisable, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
    }

	//Outside Camera
	pass p4
    {
		// Set VS, GS, and PS
        SetVertexShader( CompileShader( vs_4_0, VSScene() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, PSScene() ) );
		
		SetRasterizerState(frontCulling);
		SetDepthStencilState( BackFaceDepthStencil, 0 );
	    //SetDepthStencilState( zWriteNoStencilEnable, 0 );
		SetBlendState(AdditiveBlending, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
    }
}