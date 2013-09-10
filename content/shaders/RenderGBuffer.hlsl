Texture2D stoneFloorTex_ : register( t0 );
Texture2D colorBuffer : register( t1 );
Texture2D normalBuffer : register( t2 );
Texture2D depthBuffer : register ( t3 );

SamplerState colorSampler_ : register( s0 );

cbuffer cbPerObject
{
	float4x4 gWorldViewProj; 
	float4x4 gWorld;
	float2 gHalfPixel;
};

struct VertexIn
{
	float3 pos  : POSITION;
	float3 normal : NORMAL;
	float2 tex :	TEXCOORD;
};

struct VertexOut
{
	float4 pos  : SV_POSITION;
	float3 normal : NORMAL;
	float2 tex :	TEXCOORD;
};

struct GBufferOut
{
	half4 color : SV_TARGET0;
	half4 normal : SV_TARGET1;
};

VertexOut vs_main(VertexIn input)
{
    VertexOut output;
    
    output.pos = mul(float4(input.pos, 1.0f), gWorldViewProj);
    output.tex = input.tex;                            //pass the texture coordinates further
    output.normal = input.normal;
	
    return output;
}

GBufferOut ps_main(VertexOut input)
{
    GBufferOut output;
    output.color = stoneFloorTex_.Sample( colorSampler_, input.tex ); //output Color
    output.normal.rgb = 0.5f * (normalize(input.normal) + 1.0f);      //transform normal domain
	output.normal.a = 1.0f;
	
    return output;
}

VertexOut vs_fsquad(VertexIn input)
{
    VertexOut output;
    
    output.pos = float4(input.pos, 1.0f);
    output.tex = input.tex;                            //pass the texture coordinates further
    output.normal = input.normal;
	
    return output;
}

float4 ps_directional(VertexOut pin) : SV_TARGET0
{
    float4 n = normalBuffer.Sample( colorSampler_, pin.tex );
	float3 normal = 2.0f * n.xyz - 1.0f; //transform back into (-1, 1)
	
	float lightIntensity;
	float3 lightDir;

	float4 color = colorBuffer.Sample( colorSampler_, pin.tex );
	float4 ambient = color * 0.3f;
	
	lightDir = float3(0.0f,0.1f,0.1f);
	lightDir = normalize(lightDir);
	
	// Calculate the amount of light on this pixel.
    lightIntensity = dot(normal, lightDir);
	
	//if(lightIntensity > 0)
	//{
		color = saturate(lightIntensity * color + ambient);
	//}
	
    return color;
}

technique11 GeoPass
{
    pass P0
    {
        SetVertexShader( CompileShader( vs_5_0, vs_main() ) );
		SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_5_0, ps_main() ) );
    }
}

technique11 DirLight
{
    pass P0
    {
        SetVertexShader( CompileShader( vs_5_0, vs_fsquad() ) );
		SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_5_0, ps_directional() ) );
    }
}