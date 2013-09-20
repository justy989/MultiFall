//ui.fx: Render the UI in Multifall

Texture2D uiTexture : register( t0 );

SamplerState pointSampler
{
	Filter = MIN_MAG_MIP_POINT;
	ComparisonFunc = NEVER;
	AddressU = WRAP;
	AddressV = WRAP;
};

BlendState blendState
{
	BlendEnable[0] = TRUE;
	BlendOp[0] = ADD;
	SrcBlend[0] = SRC_ALPHA;
	DestBlend[0] = INV_SRC_ALPHA;
	RenderTargetWriteMask[0] = 0xFF;
	SrcBlendAlpha[0] = ONE;
	DestBlendAlpha[0] = ONE;
	BlendOpAlpha[0] = ADD;
};

cbuffer cbPerObject
{
	float4x4 gWorld;
};

struct VertexIn
{
	float4 PosL  : POSITION;
    float4 Color : COLOR;
	float2 Tex	 : TEXCOORD0;
};

struct VertexOut
{
	float4 PosH  : SV_POSITION;
    float4 Color : COLOR;
	float2 Tex	 : TEXCOORD0;
};

VertexOut VS_Main(VertexIn vin)
{
	VertexOut vout =( VertexOut )0;	
	
	//transform to desired screen location
	float4 pos = mul(vin.PosL, gWorld);
	vout.PosH = pos;

	// Just pass vertex color into the pixel shader.
    vout.Color = vin.Color;
	vout.Tex = vin.Tex;
	
    return vout;
}

float4 PS_Main(VertexOut pin) : SV_Target
{
	//Sample the texture
    float4 color = uiTexture.Sample( pointSampler, pin.Tex );
	
	//Multiply the sampled texture by the input color
    color *= pin.Color;

    //Cut the pixel if the alpha is too low
    clip( color.a < 0.1f ? -1 : 1 );

    return color;
}

technique11 RenderUI
{
	pass P0
	{
		SetBlendState( blendState, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF );
		SetVertexShader( CompileShader( vs_5_0, VS_Main() ) );
		SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_5_0, PS_Main() ) );
	}
}