//***************************************************************************************
// color.fx by Frank Luna (C) 2011 All Rights Reserved.
//
// Transforms and colors geometry.
//***************************************************************************************

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
};

struct VertexOut
{
	float4 PosH  : SV_POSITION;
    float4 Color : COLOR;
	float3 Normal : NORMAL;
};

VertexOut VS(VertexIn vin)
{
	VertexOut vout;
	
	// Transform to homogeneous clip space.
	vout.PosH = mul(float4(vin.PosL, 1.0f), gWorldViewProj);
	
	// Just pass vertex color into the pixel shader.
    vout.Color = vin.Color;    
	vout.Normal = vin.Normal;
	
    return vout;
}

float4 PS(VertexOut pin) : SV_Target
{
	float lightIntensity;
	float3 lightDir;
	
	lightDir = float3(0.2f,-0.1f,0.075f);
	lightDir = normalize(lightDir);
	
	// Calculate the amount of light on this pixel.
    lightIntensity = dot(pin.Normal, lightDir);
	
	[flatten]
	if(lightIntensity > 0)
	{
		pin.Color = 0.9f * lightIntensity * pin.Color;
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
