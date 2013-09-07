//***************************************************************************************
// color.fx by Frank Luna (C) 2011 All Rights Reserved.
//
// Transforms and colors geometry.
//***************************************************************************************

Texture2D gameFont_ : register( t0 );
SamplerState colorSampler_ : register( s0 );

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
	float4 color = gameFont_.Sample( colorSampler_, pin.Tex );

    //Cut the pixel if the alpha is too low
    clip( color.a < 0.1f ? -1 : 1 );

	return color;
}
