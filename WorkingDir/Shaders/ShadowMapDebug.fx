//=============================================================================
// drawshadowmap.fx by Frank Luna (C) 2008 All Rights Reserved.
//
// Effect used to display a shadow map on a quad.  The quad vertices are
// given in NDC space.
//=============================================================================
bool bGray = false;
SamplerState gTriLinearSam
{
	Filter = MIN_MAG_MIP_POINT;
	AddressU = Clamp;
	AddressV = Clamp;
};
RasterizerState NoCulling
{
	CullMode = NONE;
};
BlendState NoBlend
{
	BlendEnable[0] = FALSE;
};
 
TextureCube gCube;
struct VS_IN_CUBE
{
	float3 posL : POSITION;
	float3 texC : TEXCOORD;
};

struct VS_OUT_CUBE
{
	float4 posH : SV_POSITION;
	float3 texC : TEXCOORD;
};
VS_OUT_CUBE VSCUBE(VS_IN_CUBE vIn)
{
	VS_OUT_CUBE vOut;

	vOut.posH = float4(vIn.posL, 1.0f);
	
	vOut.texC = vIn.texC;
	
	return vOut;
}

float4 PSCUBE(VS_OUT_CUBE pIn) : SV_Target
{
	float4 color = gCube.Sample(gTriLinearSam, pIn.texC);
	//return float4(pIn.texC.x, pIn.texC.y, pIn.texC.z, 1);
	if(bGray)
		return float4(color.x, color.x, color.x, 1);

	return color;	
}
technique10 Technique1
{
    pass P0
    {
        SetVertexShader( CompileShader( vs_4_0, VSCUBE() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, PSCUBE() ) );
		SetRasterizerState( NoCulling );
		SetBlendState(NoBlend, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
    }
}