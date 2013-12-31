Texture2D tColorMap;
Texture2D tLightMap;
Texture2D tDepthMap;

SamplerState PointClampSampler
{
	Filter = MIN_MAG_MIP_POINT;
	AddressU = Clamp;
	AddressV = Clamp;
};

SamplerState LinearClampSampler
{
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = Clamp;
	AddressV = Clamp;
};

RasterizerState BackCulling
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

struct VSSceneIn
{
	float3 Position : POSITION;
	float2 TexCoord : TEXCOORD;
};

struct PSSceneIn
{
	float4 Position : SV_Position;
	float2 TexCoord : TEXCOORD;
};

//-----------------------------------------------------------------------------------------
// Constant Buffers (where we store variables by frequency of update)
//-----------------------------------------------------------------------------------------
cbuffer cbEveryFrame
{

};

//-----------------------------------------------------------------------------------------
// VertexShader: VSScene
//-----------------------------------------------------------------------------------------
PSSceneIn VSScene(VSSceneIn input)
{
	PSSceneIn output = (PSSceneIn)0;
	
	output.Position = float4(input.Position, 1.0f);

	//Align texture coordinates
	output.TexCoord = input.TexCoord;

	return output;
}

//-----------------------------------------------------------------------------------------
// PixelShader: PSSceneMain
//-----------------------------------------------------------------------------------------
float4 PSScene(PSSceneIn input) : SV_Target
{	
	float3 diffuseColor = tColorMap.Sample(LinearClampSampler, input.TexCoord).rgb;

	float4 light = tLightMap.Sample(LinearClampSampler, input.TexCoord);

	float3 diffuseLight = light.rgb;

	float specularLight = light.a;

	float3 finalColor = diffuseColor * diffuseLight + specularLight;

	float depthVal = tDepthMap.Sample(PointClampSampler, input.TexCoord).r;

	if(depthVal == 1.0f)
		finalColor = diffuseColor;

	return float4(finalColor, 1.0f);
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
        SetPixelShader( CompileShader( ps_4_0, PSScene() ) );
		
		SetRasterizerState( BackCulling );

	    SetDepthStencilState( zWriteStencilDS, 0 );
		SetBlendState(NoBlend, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
    }
}