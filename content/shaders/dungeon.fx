Texture2D texture_ : register( t0 );
SamplerState sampler_ : register( s0 );

cbuffer cbPerObject
{
	float4x4 gWorldViewProj; 
	float4x4 gWorld;
};

struct VertexIn
{
	float3 PosL  : POSITION;
	float3 Normal : NORMAL;
	float2 Tex :	TEXCOORD0;
};

struct VertexOut
{
	float4 PosH  : SV_POSITION;
	float3 Normal : NORMAL;
	float2 Tex :	TEXCOORD0;
};

VertexOut VS(VertexIn vin)
{
	VertexOut vout;
	
	// Transform to homogeneous clip space.
	vout.PosH = mul(float4(vin.PosL, 1.0f), gWorldViewProj);
	
	vout.Normal = vin.Normal;
	vout.Tex = vin.Tex;
	
    return vout;
}

float4 PS(VertexOut pin) : SV_Target
{
    //Directional Light attributes
	float lightIntensity;
	float3 lightDir;

	float4 finalColor = texture_.Sample( sampler_, pin.Tex );
	
	lightDir = float3(0.2f, -0.1f, 0.0f);
	lightDir = normalize(lightDir);
	
	// Calculate the amount of light on this pixel.
    lightIntensity = dot(pin.Normal, lightDir);
	
    [flatten]
	if(lightIntensity > 0)
	{
		finalColor = lightIntensity * finalColor;
	}
	
    return finalColor;
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
