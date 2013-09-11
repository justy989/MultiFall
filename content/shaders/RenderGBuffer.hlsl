Texture2D stoneFloorTex_ : register( t0 );
Texture2D colorBuffer : register( t1 );
Texture2D normalBuffer : register( t2 );
Texture2D depthBuffer : register ( t3 );

SamplerState colorSampler_ : register( s0 );

cbuffer cbPerObject : register (b0)
{
	float4x4 gWorldViewProj;	
};

cbuffer cbPerLightPS : register(b1)
{
	float4x4 gInvViewProj; //64 bytes
	float4 lightPosition;	//16 bytes
	float4 lightRadIntensity; //16 bytes
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

struct PointVertexOut
{
	float4 pos  : SV_POSITION;
	float4 screenPos :	TEXCOORD;
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

PointVertexOut vs_point(VertexIn input)
{
    PointVertexOut output;
    
    output.pos = mul(float4(input.pos, 1.0f), gWorldViewProj);
    output.screenPos = output.pos;
	
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
	normal = normalize(normal);
	
	float lightIntensity;
	float3 lightDir;

	float4 color = colorBuffer.Sample( colorSampler_, pin.tex );
	float4 ambient = color * 0.1f;
	
	lightDir = float3(0.1f,0.02f,-0.05f);
	lightDir = normalize(lightDir);
	
	// Calculate the amount of light on this pixel.
    lightIntensity = saturate(dot(normal, lightDir));
	
	//if(lightIntensity > 0)
	//{
		color = saturate(lightIntensity * color + ambient);
	//}
	
    return color;
}

float4 ps_point(PointVertexOut pin) : SV_TARGET0
{
	pin.screenPos.xy /= pin.screenPos.w;
	float2 texCoord = 0.5f * (float2(pin.screenPos.x, -pin.screenPos.y) + 1);
	
    float4 n = normalBuffer.Sample( colorSampler_, texCoord );
	float3 normal = 2.0f * n.xyz - 1.0f; //transform back into (-1, 1)
	normal = normalize(normal);
	
	float depth = depthBuffer.Sample( colorSampler_, texCoord ).r;
	
	float4 position;
    position.xy = pin.screenPos.xy;
    position.z = depth;
    position.w = 1.0f;
	
	//transform to world space
    position = mul(position, gInvViewProj);
    position /= position.w;
	
	//surface-to-light vector
    float3 lightVector = lightPosition - position;
	
    //compute attenuation based on distance - linear attenuation
    float attenuation = saturate(1.0f - length(lightVector)/lightRadIntensity.x); 
	
    //normalize light vector
    lightVector = normalize(lightVector); 
	
    //compute diffuse light
    float NdL = saturate(dot(normal,lightVector));
	float3 color = colorBuffer.Sample( colorSampler_, texCoord );
    float3 diffuseLight = color;//NdL * color;
	
	float4 colorfinal = float4(attenuation * lightRadIntensity.y * diffuseLight, 1.0f);
	
    return colorfinal;
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

technique11 PointLight
{
    pass P0
    {
        SetVertexShader( CompileShader( vs_5_0, vs_point() ) );
		SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_5_0, ps_point() ) );
    }
}