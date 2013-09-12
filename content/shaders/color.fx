//***************************************************************************************
// color.fx by Frank Luna (C) 2011 All Rights Reserved.
//
// Transforms and colors geometry.
//***************************************************************************************

Texture2D stoneFloorTex_ : register( t0 );
//Texture2D stoneWallTex_ : register( t1 );
SamplerState colorSampler_ : register( s0 );

cbuffer cbPerObject
{
	float4x4 gWorldViewProj; 
	float4x4 gWorld;
};

struct VertexIn
{
	float3 PosL  : POSITION;
    float4 Color : COLOR;
	float3 Normal : NORMAL;
	float2 Tex :	TEXCOORD0;
};

struct VertexOut
{
	float4 PosH  : SV_POSITION;
    float4 Color : COLOR;
	float3 Normal : NORMAL;
	float2 Tex :	TEXCOORD0;
};

VertexOut VS(VertexIn vin)
{
	VertexOut vout;
	
	// Transform to homogeneous clip space.
	vout.PosH = mul(float4(vin.PosL, 1.0f), gWorldViewProj);
	
	// Just pass vertex color into the pixel shader.
    vout.Color = vin.Color;    
	vout.Normal = vin.Normal;
	vout.Tex = vin.Tex;
	
    return vout;
}

float4 PS(VertexOut pin) : SV_Target
{
	float lightIntensity;
	float3 lightDir;

	//pin.Color = pin.Color * stoneFloorTex_.Sample( colorSampler_, pin.Tex );
	pin.Color = stoneFloorTex_.Sample( colorSampler_, pin.Tex );
	
	lightDir = float3(0.2f,-0.1f,0);
	lightDir = normalize(lightDir);
	
	// Calculate the amount of light on this pixel.
    lightIntensity = dot(pin.Normal, lightDir);
	
	if(lightIntensity > 0)
	{
		pin.Color = lightIntensity * pin.Color;
	}
	
    return pin.Color;
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
