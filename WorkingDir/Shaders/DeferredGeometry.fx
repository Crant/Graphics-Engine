Texture2D tex2D;
Texture2D normalMap;

SamplerState AnisotropicWrapSampler 
{
	Filter = ANISOTROPIC;
	AddressU = Wrap;
	AddressV = Wrap;
	AddressW = Wrap;
	MipLODBias = 0.0f;//Unused
	MaxAnisotropy = 16;
	ComparisonFunc = ALWAYS; 
	BorderColor = float4(0.0f, 0.0f, 0.0f, 0.0f); //Unused
	MinLOD = 0.0f;
	MaxLOD = 16.0f; //Max
};

SamplerState LinearSampler
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Wrap;
    AddressV = Wrap;
};

RasterizerState BackCulling
{
	CullMode = Back;
	FillMode = Solid;
};

DepthStencilState EnableDepth
{
    DepthEnable = TRUE;
    DepthWriteMask = ALL;
    DepthFunc = LESS_EQUAL;
};

BlendState NoBlend
{
	BlendEnable[0] = FALSE;
};

struct VSSceneIn
{
	float3 Position : POSITION;
	float3 Tangent : TANGENT;
	float3 Normal : NORMAL;
	float2 TexCoord : TEXCOORD;
};

struct PSSceneIn
{
	float4 Position : SV_Position;
	float2 TexCoord : TEXCOORD0;
	float3 Normal : NORMAL;
	float3 Depth : TEXCOORD1;
	float3 Tangent : TANGENT;
	float3 Binormal : BINORMAL;
};

struct PSSceneOut
{
    float4 Color : SV_TARGET0;
	float4 Normal : SV_TARGET1;
	float4 Depth : SV_TARGET2;
};
/*
RTs
1: Color XYZ, W Spec intensity
2: Normal XYZ, W Spec Power
3: Depth X Screen Space depth
*/

//Normal Encoding Function
float3 encode(float3 normal)
{
	normal = normalize(normal);

	normal.xyz = 0.5f * (normal.xyz + 1.0f);

	return normal;
}

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
	matrix mView;
	matrix mProjection;
};

cbuffer cbEveryStrip
{
	matrix mWorld;
	matrix mWorldInverseTranspose;
	bool useNormalMap;
	bool useSpecMap;

	float specularIntensity = 0.1f;
	float specularPower = 0.5f;
}

//-----------------------------------------------------------------------------------------
// VertexShader: VSScene
//-----------------------------------------------------------------------------------------

PSSceneIn VSScene(VSSceneIn input)
{
	PSSceneIn output = (PSSceneIn)0;
	
	float4 worldPos = mul(float4(input.Position, 1.0f), mWorld);
	float4 viewPos = mul(worldPos, mView);
	output.Position = mul(viewPos, mProjection);

	output.TexCoord = input.TexCoord;

	output.Normal = mul(input.Normal, (float3x3)mWorldInverseTranspose);

	output.Depth.x = output.Position.z;
	output.Depth.y = output.Position.w;
	output.Depth.z = viewPos.z;

	float3 Binormal = normalize(cross(input.Normal,input.Tangent));

	output.Tangent = normalize(mul(input.Tangent, (float3x3)mWorldInverseTranspose));
	output.Binormal = normalize(mul(Binormal, (float3x3)mWorldInverseTranspose));

	return output;
}

//-----------------------------------------------------------------------------------------
// PixelShader: PSSceneMain
//-----------------------------------------------------------------------------------------
PSSceneOut PSScene(PSSceneIn input) : SV_Target
{	
	PSSceneOut output = (PSSceneOut)0;
	
	//Sample textures
	float4 textureColor = tex2D.Sample( AnisotropicWrapSampler, input.TexCoord );
	if(textureColor.a < 0.5f)
		discard;

	output.Color.rgb = textureColor.rgb;
	
	//Transform normal
	if(!useNormalMap)
	{
		output.Normal.xyz = encode(input.Normal);
	}
	else
	{
		float3 bumpMap = normalMap.Sample(AnisotropicWrapSampler, input.TexCoord).xyz;
		// Expand the range of the normal value from (0, +1) to (-1, +1).
		bumpMap = decode(float4(bumpMap, 1.0f));
		// Calculate the normal from the data in the bump map.
		float3 bumpNormal = input.Normal + bumpMap.x * input.Tangent + bumpMap.y * input.Binormal;
		output.Normal.xyz = encode(normalize(bumpNormal));
	}

	if(!useSpecMap)
	{
		//output specularIntensity
		output.Color.a = specularIntensity;

		//output specularPower
		output.Normal.a = specularPower;
	}
	else
	{
		//float4 specular = tSpecularMap.Sample(LinearSampler, input.TexCoord);

		////output specularIntensity
		//output.Color.a = (specular.r + specular.g + specular.b) / 3;

		////output specularPower
		//output.Normal.a = specular.a;
	}

	//depth(Screen Space, for lighting)
	output.Depth.x = input.Depth.x / input.Depth.y;
	//depth(View Space, for SSAO)
	//output.Depth.y = input.Depth.z;

	return output;
}
//-----------------------------------------------------------------------------------------
// Technique: RenderTextured  
//-----------------------------------------------------------------------------------------

technique11 BasicTech
{
    pass p0
    {
		// Set VS, GS, and PS
        SetVertexShader( CompileShader( vs_4_0, VSScene() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, PSScene() ) );
	    

		SetDepthStencilState( EnableDepth, 0 );
	    SetRasterizerState( BackCulling );
		SetBlendState(NoBlend, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
    }  
}