//***************************************************************************************
// color.fx by Frank Luna (C) 2011 All Rights Reserved.
//
// Transforms and colors geometry.
//***************************************************************************************

Texture2D gameFont_ : register( t0 );
SamplerState colorSampler_ : register( s0 );

struct VertexIn
{
	float3 PosL  : POSITION0;
    float4 Color : COLOR0;
	float2 Tex	 : TEXCOORD0;
	float3 Normal : NORMAL; //only used ofr padding
};

struct VertexOut
{
	float3 PosH  : SV_POSITION;
    float4 Color : COLOR0;
	float2 Tex	 : TEXCOORD0;
	float3 Normal : NORMAL; //only used ofr padding
};

VertexOut VS(VertexIn vin)
{
	VertexOut vout;	
	
	// Just pass vertex color into the pixel shader.
    vout.Color = vin.Color;
	
    return vout;
}

float4 PS(VertexOut pin) : SV_Target
{
	return gameFont_.Sample( colorSampler_, pin.Tex );
}

technique11 ColorTech
{
    pass P0
    {
        SetVertexShader( CompileShader( vs_5_0, VS() ) );
		SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_5_0, PS() ) );
    }
}
