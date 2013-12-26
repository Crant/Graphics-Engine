
RasterizerState backCulling
{
    CullMode = Back;
	FillMode = Solid;
};
BlendState BlendDisabled
{
    RenderTargetWriteMask[0] = 0x0f;
    BlendEnable[0] = false;
};
DepthStencilState zWriteStencil
{
    DepthEnable = true;
    DepthFunc = Less_Equal;
    DepthWriteMask = All;
    StencilEnable = true;
};

struct VS_IN_CUBE
{
    float3 Position : POSITION;
};
struct PS_IN_CUBE
{
    float4 Position : SV_Position;
    float Depth : TEXCOORD;
	uint RTIndex : SV_rendertargetarrayindex;
};
struct GS_IN_CUBE
{
	float4 Position : SV_Position;
};
cbuffer cbPerMesh
{
    matrix mWorld;
	matrix mProj;

	float3 gLightPos;
};
cbuffer cbPerView
{
	matrix mView;
}
cbuffer cbGeoPerMesh
{
	matrix mLightView[6];
}

GS_IN_CUBE VSCubeGenerateVSM( VS_IN_CUBE input )
{
    GS_IN_CUBE output = (GS_IN_CUBE)0.0f;
    
	output.Position = mul(float4(input.Position, 1.0f), mWorld);

    return output;
}
[maxvertexcount(18)]
void GSCubeGenerateVSM(triangle GS_IN_CUBE input[3], inout TriangleStream<PS_IN_CUBE> CubeMapStream)
{
	// For each face of the cube, create a new triangle
 	//--
 	[unroll]
	for( int f = 0; f < 6; ++f )
    {
        // Compute screen coordinates
        PS_IN_CUBE output;
		output.RTIndex = f;
	 	// For each vertex of the triangle, compute screen space position and pass distance
	 	//--
	 	[unroll]
        for( int v = 0; v < 3; v++ )
        {
			float4 eyePos = mul(input[v].Position, mLightView[f]);
            output.Position = mul(eyePos, mProj);
			output.Depth = length(gLightPos.xyz - input[v].Position.xyz);
			
            CubeMapStream.Append( output );
        }
        CubeMapStream.RestartStrip();
    }
}
float4 PSCubeGenerateVSM( PS_IN_CUBE input ) : SV_Target
{
    return float4(input.Depth, 1, 1.0f, 1.0f );
}

technique10 Technique1
{
    pass p0
    {
        SetVertexShader( CompileShader( vs_4_0, VSCubeGenerateVSM() ) );
        SetGeometryShader( CompileShader(gs_4_0, GSCubeGenerateVSM()) );
        SetPixelShader( CompileShader( ps_4_0, PSCubeGenerateVSM() ) );
        
        SetRasterizerState( backCulling );
        SetBlendState( BlendDisabled, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xffffffff );
        SetDepthStencilState( zWriteStencil, 0xffffffff );
    }
}


struct VSIN
{
    float3 pos : POSITION;
};

struct PSIN
{
    float4 pos : SV_Position;
    float gDepth : TEXCOORD;
};

PSIN VSGenerateVSM( VSIN input )
{
    PSIN output = (PSIN)0;
    
    float4 worldPos = mul(float4(input.pos, 1.0f), mWorld);
    
	float4 eyePos = mul(worldPos, mView);
	output.pos = mul(eyePos, mProj);
	float3 lightVector = worldPos.xyz - gLightPos;
    output.gDepth = length(lightVector);

    return output;
}
float4 PSGenerateVSM( PSIN input ) : SV_Target
{
	float depth = input.gDepth;
	return float4(depth, 1, 1, 1);
}

technique10 Technique2
{
    pass p0
    {
        SetVertexShader( CompileShader( vs_4_0, VSGenerateVSM() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, PSGenerateVSM() ) );
        
        SetRasterizerState( backCulling );
        SetBlendState( BlendDisabled, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xffffffff );
        SetDepthStencilState( zWriteStencil, 0xffffffff );
    }
}